const MAX_MESSAGE_CHARS = 4000;

const SYSTEM_INSTRUCTIONS = [
  "You are Voidlight, a helpful Geometry Dash 2.2 editor assistant.",
  "Help with level decoration, color palettes, glow, object usage, gameplay readability,",
  "level planning, music mood matching, and safe editing workflows.",
  "Give concise practical steps. Never claim you changed a level or placed objects.",
  "When asked for Auto Decor, return a clear decoration plan; the game must preview",
  "and ask for confirmation before modifying or deleting any level objects.",
  "If the request is unrelated, answer normally and briefly."
].join(" ");

function json(data, status = 200, origin = "*") {
  return new Response(JSON.stringify(data), {
    status,
    headers: {
      "content-type": "application/json; charset=utf-8",
      "cache-control": "no-store",
      "access-control-allow-origin": origin,
      "access-control-allow-methods": "POST, OPTIONS",
      "access-control-allow-headers": "Content-Type"
    }
  });
}

function extractText(data) {
  if (typeof data.output_text === "string" && data.output_text.length) {
    return data.output_text;
  }
  const parts = [];
  for (const item of (data.output || [])) {
    if (item.type !== "message") continue;
    for (const part of (item.content || [])) {
      if (part.type === "output_text" && typeof part.text === "string") {
        parts.push(part.text);
      }
    }
  }
  return parts.join("\n").trim();
}

export default {
  async fetch(request, env) {
    const origin = env.ALLOWED_ORIGIN || "*";
    if (request.method === "OPTIONS") {
      return new Response(null, {
        status: 204,
        headers: {
          "access-control-allow-origin": origin,
          "access-control-allow-methods": "POST, OPTIONS",
          "access-control-allow-headers": "Content-Type",
          "access-control-max-age": "86400"
        }
      });
    }

    const url = new URL(request.url);
    if (url.pathname === "/health" && request.method === "GET") {
      return json({ ok: true, service: "voidlight-ai" }, 200, origin);
    }
    if (url.pathname !== "/chat" || request.method !== "POST") {
      return json({ error: "Not found" }, 404, origin);
    }
    if (!env.OPENAI_API_KEY) {
      return json({ error: "AI service is not configured yet." }, 503, origin);
    }

    let body;
    try {
      const raw = await request.text();
      if (raw.length > 12000) return json({ error: "Request is too large." }, 413, origin);
      body = JSON.parse(raw);
    } catch {
      return json({ error: "Invalid JSON request." }, 400, origin);
    }

    const message = typeof body.message === "string" ? body.message.trim() : "";
    if (!message) return json({ error: "Write a message first." }, 400, origin);
    if (message.length > MAX_MESSAGE_CHARS) {
      return json({ error: "Message is too long (maximum 4000 characters)." }, 413, origin);
    }

    try {
      const upstream = await fetch("https://api.openai.com/v1/responses", {
        method: "POST",
        headers: {
          "authorization": `Bearer ${env.OPENAI_API_KEY}`,
          "content-type": "application/json"
        },
        body: JSON.stringify({
          model: env.OPENAI_MODEL || "gpt-6-astra",
          instructions: SYSTEM_INSTRUCTIONS,
          input: message,
          max_output_tokens: 700
        })
      });

      const data = await upstream.json().catch(() => ({}));
      if (!upstream.ok) {
        // Do not forward provider internals, credentials, or raw error payloads to the game.
        return json({
          error: upstream.status === 429
            ? "AI is busy or the API usage limit was reached. Try again later."
            : "The AI service returned an error. Check the server configuration."
        }, upstream.status === 429 ? 429 : 502, origin);
      }

      const answer = extractText(data);
      if (!answer) return json({ error: "The AI returned an empty response. Try again." }, 502, origin);
      return json({ answer }, 200, origin);
    } catch {
      return json({ error: "Could not reach the AI service. Check the connection and try again." }, 502, origin);
    }
  }
};
