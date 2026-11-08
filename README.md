# GIGA-CHAT-MOD — Geode Bridge

A background companion mod for Geometry Dash 2.2 and Geode, with a private relay server.

## Current implementation

- The Geode client starts when Geometry Dash initializes its main menu.
- It adds no gameplay buttons, popups, overlays, or editor UI.
- The mod settings contain two configuration fields: the HTTPS bridge server URL and a one-time pairing code.
- The client can claim a pairing code, store its device token in Geode's app-private mod save data, poll for commands, and acknowledge results.
- The currently enabled commands are `bridge.ping` and `editor.get_status`.
- `bridge.ping` is implemented.
- `editor.get_status` currently returns an explicit placeholder (`editor: "unknown"`); actual editor-state detection is not implemented.
- No level data is read or modified. No level-editing commands are enabled.

## Build status

GitHub Actions builds Windows, Android 64-bit, and Android 32-bit targets and packages the artifacts. A successful workflow means those configured targets compiled and the package step completed; it does not mean the mod has been installed or tested inside Geometry Dash on a physical phone.

## Bridge server

The Node.js 24 relay is in `bridge/`. It provides one-time device pairing, authenticated device polling, command acknowledgements, assistant-authenticated command queueing, and command-result retrieval.

The server accepts only `bridge.ping` and `editor.get_status`. It stores device-token hashes in SQLite. Pairing and assistant credentials must be configured as private hosting environment variables:

- `BRIDGE_PAIRING_TOKEN`
- `BRIDGE_ASSISTANT_TOKEN`

Use separate long random values. Never commit them, place them in the mod, or paste them into chat. The deployment blueprint is `render.yaml`; a persistent disk is required for the SQLite database.

## Pairing after private deployment

1. Deploy the relay and configure both private environment secrets.
2. Use the owner-authenticated `POST /v1/pair/start` endpoint to generate a five-minute code.
3. Open Geode's settings for this mod (not a new in-game menu).
4. Enter the HTTPS server URL and the six-digit one-time pairing code.
5. Restart Geometry Dash so the client can claim the code and begin polling.
6. Revoke the paired device on the server if access should stop.

The raw device token is stored in Geode's app-private mod save data. This is sandboxed by the mobile OS, but it is not a hardware-backed encrypted keystore.

## ChatGPT integration limitation

This repository does not automatically connect an ordinary ChatGPT conversation to the relay. That final connection requires a separately configured, authorized ChatGPT Action/connector or another client that can authenticate to the assistant API. Do not treat a successful mod build as proof that this chat can control the game.

## Development checks

From `bridge/`, run:

```sh
npm run check
```

This runs JavaScript syntax checks and the bridge smoke tests. The Geode client is built by the GitHub Actions workflow in `.github/workflows/multi-platform.yml`.
