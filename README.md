# Geode Bridge

A background companion mod for Geometry Dash 2.2 and Geode.

## Current milestone

- The mod initializes automatically when Geometry Dash reaches its main menu.
- It adds no in-game buttons, popups, overlays, or other UI.
- It does not inspect or modify levels yet.
- It does not send gameplay data.
- The HTTPS health-check client is scaffolded, but remains inactive until a real bridge server endpoint is configured.

## Secure connection status

The client refuses non-HTTPS endpoints and uses Geode's asynchronous WebRequest API, so network activity does not block the game thread. The endpoint is intentionally empty in this prototype. This is deliberate: the project does not yet have a deployed server, and the mod must not send data to an unknown or placeholder host.

A working assistant connection still requires a separately deployed bridge server and an explicit pairing/authorization flow. Installing the mod cannot attach it directly to an existing ChatGPT conversation.

## Privacy

- No account passwords, ChatGPT credentials, or API keys are collected.
- No gameplay data or level contents are uploaded.
- No connection is attempted while the server endpoint is unset.
- Future data sharing must be limited, disclosed, and authorized by the player.
- The first editor integration should be read-only until level-editing commands are designed and reviewed.

## Build

This repository uses the Geode SDK and GitHub Actions workflow in `.github/workflows/multi-platform.yml`.
