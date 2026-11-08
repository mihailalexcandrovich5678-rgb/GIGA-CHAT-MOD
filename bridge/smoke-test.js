import { spawn } from "node:child_process";
import { createServer } from "node:net";
import { mkdtemp, rm } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import assert from "node:assert/strict";

const directory = await mkdtemp(join(tmpdir(), "geode-bridge-test-"));
const assistantToken = "test-assistant-token-which-is-long-enough";
const pairingToken = "test-pairing-token-which-is-different";
const portServer = createServer();
await new Promise(resolve => portServer.listen(0, "127.0.0.1", resolve));
const port = portServer.address().port;
await new Promise(resolve => portServer.close(resolve));

const child = spawn(process.execPath, ["server.js"], {
  cwd: new URL(".", import.meta.url),
  env: {
    ...process.env,
    HOST: "127.0.0.1",
    PORT: String(port),
    BRIDGE_DB_PATH: join(directory, "test.sqlite"),
    BRIDGE_ASSISTANT_TOKEN: assistantToken,
    BRIDGE_PAIRING_TOKEN: pairingToken
  },
  stdio: "ignore"
});
const base = `http://127.0.0.1:${port}`;
const request = async (path, options = {}) => {
  const response = await fetch(base + path, options);
  const body = await response.json();
  return { status: response.status, body };
};
const auth = token => ({ authorization: `Bearer ${token}` });

try {
  let healthy = false;
  for (let i = 0; i < 100; i++) {
    try {
      if ((await request("/health")).status === 200) { healthy = true; break; }
    } catch {}
    await new Promise(resolve => setTimeout(resolve, 100));
  }
  assert.equal(healthy, true, "server should start and report healthy");

  const started = await request("/v1/pair/start", {
    method: "POST", headers: auth(pairingToken)
  });
  assert.equal(started.status, 201);
  const claimed = await request("/v1/pair/claim", {
    method: "POST", headers: { "content-type": "application/json" },
    body: JSON.stringify({ code: started.body.pairing_code })
  });
  assert.equal(claimed.status, 201);
  const deviceToken = claimed.body.device_token;
  const deviceAuth = auth(deviceToken);
  assert.equal((await request("/v1/device/status", { headers: deviceAuth })).status, 200);

  const listed = await request("/v1/assistant/devices", { headers: auth(assistantToken) });
  assert.equal(listed.status, 200);
  assert.equal(listed.body.devices[0].id, claimed.body.device_id);

  const queued = await request("/v1/assistant/commands", {
    method: "POST",
    headers: { ...auth(assistantToken), "content-type": "application/json" },
    body: JSON.stringify({ device_id: claimed.body.device_id, kind: "bridge.ping" })
  });
  assert.equal(queued.status, 202);

  const polled = await request("/v1/device/commands", { headers: deviceAuth });
  assert.equal(polled.status, 200);
  assert.equal(polled.body.commands[0].id, queued.body.command_id);

  const ack = await request(`/v1/device/commands/${queued.body.command_id}/ack`, {
    method: "POST",
    headers: { ...deviceAuth, "content-type": "application/json" },
    body: JSON.stringify({ status: "completed", result: { pong: true, protocol: 1 } })
  });
  assert.equal(ack.status, 200);

  const result = await request(`/v1/assistant/commands/${queued.body.command_id}`, {
    headers: auth(assistantToken)
  });
  assert.equal(result.status, 200);
  assert.deepEqual(result.body.result, { pong: true, protocol: 1 });
  assert.equal(result.body.status, "completed");

  const revoked = await request("/v1/device/revoke", { method: "POST", headers: deviceAuth });
  assert.equal(revoked.status, 200);
  assert.equal((await request("/v1/device/status", { headers: deviceAuth })).status, 401);
  process.stdout.write("Bridge smoke test passed.\n");
} finally {
  child.kill();
  await rm(directory, { recursive: true, force: true });
}
