import http from "node:http";
import { randomBytes, randomInt, createHash, timingSafeEqual } from "node:crypto";
import { DatabaseSync } from "node:sqlite";

const PORT = Number(process.env.PORT || 8787);
const HOST = process.env.HOST || "0.0.0.0";
const ASSISTANT_TOKEN = process.env.BRIDGE_ASSISTANT_TOKEN || "";
const DB_PATH = process.env.BRIDGE_DB_PATH || "./bridge.sqlite";
const MAX_BODY = 16 * 1024;
const db = new DatabaseSync(DB_PATH);

db.exec(`
  PRAGMA journal_mode = WAL;
  CREATE TABLE IF NOT EXISTS pair_codes (
    code_hash TEXT PRIMARY KEY, expires_at INTEGER NOT NULL, used INTEGER NOT NULL DEFAULT 0
  );
  CREATE TABLE IF NOT EXISTS devices (
    id TEXT PRIMARY KEY, token_hash TEXT NOT NULL, created_at INTEGER NOT NULL,
    last_seen INTEGER, revoked INTEGER NOT NULL DEFAULT 0
  );
  CREATE TABLE IF NOT EXISTS commands (
    id TEXT PRIMARY KEY, device_id TEXT NOT NULL, kind TEXT NOT NULL,
    payload TEXT NOT NULL, status TEXT NOT NULL DEFAULT 'queued',
    created_at INTEGER NOT NULL, claimed_at INTEGER, completed_at INTEGER
  );
`);

const hash = value => createHash("sha256").update(value).digest("hex");
const safeEqual = (a, b) => {
  const aa = Buffer.from(a); const bb = Buffer.from(b);
  return aa.length === bb.length && timingSafeEqual(aa, bb);
};
const json = (res, status, value) => {
  const body = JSON.stringify(value);
  res.writeHead(status, { "content-type": "application/json; charset=utf-8", "cache-control": "no-store" });
  res.end(body);
};
async function readBody(req) {
  let raw = "";
  for await (const chunk of req) {
    raw += chunk;
    if (Buffer.byteLength(raw) > MAX_BODY) throw new Error("body_too_large");
  }
  return raw ? JSON.parse(raw) : {};
}
function bearer(req) {
  const header = req.headers.authorization || "";
  return header.startsWith("Bearer ") ? header.slice(7) : "";
}
function deviceFor(req) {
  const token = bearer(req);
  if (!token) return null;
  const row = db.prepare("SELECT id FROM devices WHERE token_hash=? AND revoked=0").get(hash(token));
  return row || null;
}
function assistantAuthorized(req) {
  const supplied = bearer(req);
  return ASSISTANT_TOKEN && supplied && safeEqual(supplied, ASSISTANT_TOKEN);
}
function route(req, res) {
  const url = new URL(req.url, "http://localhost");
  if (req.method === "GET" && url.pathname === "/health") {
    return json(res, 200, { ok: true, service: "geode-bridge", protocol: 1 });
  }
  if (req.method === "POST" && url.pathname === "/v1/pair/start") {
    const code = String(randomInt(100000, 1000000));
    db.prepare("INSERT INTO pair_codes(code_hash, expires_at) VALUES(?,?)")
      .run(hash(code), Date.now() + 5 * 60_000);
    return json(res, 201, { pairing_code: code, expires_in_seconds: 300, note: "Keep this code private." });
  }
  if (req.method === "POST" && url.pathname === "/v1/pair/claim") {
    return readBody(req).then(body => {
      const code = String(body.code || "");
      if (!/^\d{6}$/.test(code)) return json(res, 400, { error: "invalid_pairing_code" });
      const row = db.prepare("SELECT * FROM pair_codes WHERE code_hash=? AND used=0").get(hash(code));
      if (!row || row.expires_at < Date.now()) return json(res, 401, { error: "pairing_code_invalid_or_expired" });
      const deviceId = randomBytes(12).toString("hex");
      const token = randomBytes(32).toString("base64url");
      const tx = db.transaction(() => {
        db.prepare("UPDATE pair_codes SET used=1 WHERE code_hash=?").run(hash(code));
        db.prepare("INSERT INTO devices(id,token_hash,created_at) VALUES(?,?,?)").run(deviceId, hash(token), Date.now());
      });
      tx();
      return json(res, 201, { device_id: deviceId, device_token: token, warning: "Store this token securely; it is shown only once." });
    }).catch(() => json(res, 400, { error: "invalid_json" }));
  }
  if (req.method === "GET" && url.pathname === "/v1/device/status") {
    const device = deviceFor(req);
    if (!device) return json(res, 401, { error: "unauthorized" });
    db.prepare("UPDATE devices SET last_seen=? WHERE id=?").run(Date.now(), device.id);
    return json(res, 200, { ok: true, device_id: device.id, protocol: 1 });
  }
  if (req.method === "GET" && url.pathname === "/v1/device/commands") {
    const device = deviceFor(req);
    if (!device) return json(res, 401, { error: "unauthorized" });
    const rows = db.prepare("SELECT id,kind,payload,created_at FROM commands WHERE device_id=? AND status='queued' ORDER BY created_at LIMIT 5").all(device.id);
    const claim = db.prepare("UPDATE commands SET status='delivered',claimed_at=? WHERE id=? AND status='queued'");
    const commands = rows.filter(row => Number(claim.run(Date.now(), row.id).changes) === 1)
      .map(row => ({ id: row.id, kind: row.kind, payload: JSON.parse(row.payload) }));
    db.prepare("UPDATE devices SET last_seen=? WHERE id=?").run(Date.now(), device.id);
    return json(res, 200, { commands, poll_after_ms: 2500 });
  }
  if (req.method === "POST" && url.pathname.startsWith("/v1/device/commands/") && url.pathname.endsWith("/ack")) {
    const device = deviceFor(req);
    if (!device) return json(res, 401, { error: "unauthorized" });
    const id = url.pathname.split("/")[4];
    return readBody(req).then(body => {
      const status = body.status === "completed" ? "completed" : "failed";
      const result = db.prepare("UPDATE commands SET status=?,completed_at=? WHERE id=? AND device_id=? AND status='delivered'")
        .run(status, Date.now(), id, device.id);
      return json(res, result.changes ? 200 : 404, { ok: Boolean(result.changes) });
    }).catch(() => json(res, 400, { error: "invalid_json" }));
  }
  if (req.method === "POST" && url.pathname === "/v1/assistant/commands") {
    if (!assistantAuthorized(req)) return json(res, 401, { error: "assistant_unauthorized" });
    return readBody(req).then(body => {
      const allowed = new Set(["bridge.ping", "editor.get_status"]);
      if (!allowed.has(body.kind)) return json(res, 400, { error: "command_not_enabled", allowed: [...allowed] });
      const deviceId = String(body.device_id || "");
      if (!db.prepare("SELECT id FROM devices WHERE id=? AND revoked=0").get(deviceId))
        return json(res, 404, { error: "device_not_found" });
      const id = randomBytes(12).toString("hex");
      db.prepare("INSERT INTO commands(id,device_id,kind,payload,created_at) VALUES(?,?,?,?,?)")
        .run(id, deviceId, body.kind, JSON.stringify({}), Date.now());
      return json(res, 202, { accepted: true, command_id: id, status: "queued" });
    }).catch(() => json(res, 400, { error: "invalid_json" }));
  }
  if (req.method === "POST" && url.pathname === "/v1/device/revoke") {
    const device = deviceFor(req);
    if (!device) return json(res, 401, { error: "unauthorized" });
    db.prepare("UPDATE devices SET revoked=1 WHERE id=?").run(device.id);
    return json(res, 200, { ok: true });
  }
  return json(res, 404, { error: "not_found" });
}

const server = http.createServer((req, res) => {
  Promise.resolve(route(req, res)).catch(error => {
    const status = error.message === "body_too_large" ? 413 : 500;
    json(res, status, { error: status === 413 ? "body_too_large" : "internal_error" });
  });
});
server.listen(PORT, HOST, () => console.log(`Geode Bridge listening on ${HOST}:${PORT}`));
