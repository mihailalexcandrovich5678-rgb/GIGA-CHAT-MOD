# Geode Bridge server (protocol v1)

This is a small HTTPS-ready relay service. Deploy it behind a host that provides TLS.
It uses Node.js 24 and the built-in SQLite module. Configure a persistent disk for the
SQLite database; without one, pairing and queued commands may be lost on redeploy.

## Endpoints

- `GET /health` — public health check.
- `POST /v1/pair/start` — creates a one-time, five-minute pairing code.
- `POST /v1/pair/claim` — exchanges that code for a device token (shown once).
- `GET /v1/device/status` — device-authenticated status.
- `GET /v1/device/commands` — device-authenticated long-poll substitute.
- `POST /v1/device/commands/:id/ack` — acknowledge a delivered command.
- `POST /v1/device/revoke` — revoke the calling device token.
- `POST /v1/assistant/commands` — assistant-side command enqueue, protected by
  `BRIDGE_ASSISTANT_TOKEN`.

Assistant-side requests must use `Authorization: Bearer <BRIDGE_ASSISTANT_TOKEN>`.
Device requests use the one-time device token returned by pairing.

## Safety state

Only `bridge.ping` and `editor.get_status` are accepted. No level-editing command is
enabled in this server milestone. Do not enable destructive editor operations until
their schemas, confirmation rules, and rollback behavior are reviewed.

The current Geode mod does not yet implement this protocol or persist a device token.
This server is the relay foundation, not a claim that the live ChatGPT conversation is
already connected. A ChatGPT Action/connector or another explicitly authorized client
is still required to call the assistant endpoint. Never put the assistant token in the
mod, a public repository, or a client-side app.

## Local run

Use Node.js 24:

```sh
cd bridge
npm start
```

Set `BRIDGE_ASSISTANT_TOKEN` in the server environment before using the assistant
endpoint. For production, use HTTPS and persistent storage.
