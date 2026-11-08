# Geode Bridge server (protocol v1)

This relay uses Node.js 24 and built-in SQLite. Deploy behind a host that provides HTTPS
and attach persistent storage to the database path.

## Endpoints

- `GET /health` — public health check.
- `POST /v1/pair/start` — owner-authenticated, rate-limited, five-minute pairing code.
- `POST /v1/pair/claim` — rate-limited exchange for a device token (shown once).
- `GET /v1/device/status` — device-authenticated status.
- `GET /v1/device/commands` — device-authenticated command polling.
- `POST /v1/device/commands/:id/ack` — acknowledge a delivered command and optionally return a small JSON result (maximum 8 KiB).
- `POST /v1/device/revoke` — revoke the calling device token.
- `GET /v1/assistant/devices` — list active paired device IDs and last-seen timestamps.
- `POST /v1/assistant/commands` — assistant-side enqueue, protected by
  `BRIDGE_ASSISTANT_TOKEN`.
- `GET /v1/assistant/commands/:id` — read command status and its optional JSON result.

## Required server secrets

Set both `BRIDGE_ASSISTANT_TOKEN` and `BRIDGE_PAIRING_TOKEN` as private environment
secrets on the hosting provider. Use separate, long random values. Never commit real
secrets, put them in the mod, or paste them into this chat. The pairing administrator
token protects pairing-code generation; codes expire after five minutes and can be used
only once. Pair-start and pair-claim requests are throttled per source IP in memory.
The application does not persist or log source IP addresses.

Device tokens are stored only as SHA-256 hashes in SQLite. The raw token is returned
once by the claim endpoint and must be kept in the device's private storage. A device
can revoke its own token with `POST /v1/device/revoke`.

## Safety state and integration status

Only `bridge.ping` and `editor.get_status` are accepted. No level-editing command is
enabled. The current Geode mod does not yet implement protocol polling or secure token
storage. The relay now supports assistant-side device discovery and result retrieval; device
acknowledgements can include a bounded JSON result. Existing SQLite databases are upgraded
with a `result` column on startup. This relay does not automatically connect an ordinary ChatGPT conversation:
a separately configured, authorized ChatGPT Action/connector or client is required.
Never put either server secret in the mod, a public repository, or a client-side app.

## Local run

Use Node.js 24, set both secrets in the server environment, then run:

```sh
cd bridge
npm start
```
