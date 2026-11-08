# Geode Bridge

A small, privacy-first companion mod for Geometry Dash 2.2 and Geode.

## First milestone

The initial Android64 build is a UI prototype. The intended read-only workflow is:

1. Read basic information about the currently open editor level.
2. Create a local diagnostic report that the player chooses to share in chat.
3. Let an assistant inspect the report and explain the level structure or Geode errors.

## Privacy and safety

- No automatic uploads.
- No account passwords, tokens, or private credentials are collected.
- The first version is read-only: it must not modify or delete level objects.
- Any future network connection must be opt-in and clearly show what data is sent.

## Build

This repository uses the Geode SDK and the GitHub Actions workflow in `.github/workflows/multi-platform.yml`.
