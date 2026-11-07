# Voidlight AI server

This folder contains a small Cloudflare Worker proxy for the Voidlight mod.
The Worker keeps the OpenAI API key on the server; the key must never be put
inside the Geometry Dash mod, a public repository, or a client config.

## What it does

- `GET /health` reports whether the Worker is reachable (not whether the AI key works).
- `POST /chat` accepts `{"message":"..." }` and returns `{"answer":"..." }`.
- Limits each message to 4,000 characters and each request body to 12,000 characters.
- Does not log prompts or provider response bodies.
- Uses the OpenAI Responses API and returns a friendly error instead of exposing provider internals.

## Setup required by the account owner

1. Create a Cloudflare Worker from this folder and deploy it.
2. In the Worker settings, add `OPENAI_API_KEY` as a **Secret**, never as a plain variable.
3. Set `OPENAI_MODEL` only to a model enabled for the OpenAI project.
4. Configure Cloudflare rate limiting and usage alerts before sharing the endpoint.
5. Keep API billing and usage limits under an adult account owner's supervision.

The Worker cannot be used for real AI replies until the secret and deployment are configured.
API usage is billed separately from a ChatGPT subscription.

## Request

```http
POST /chat
Content-Type: application/json

{"message":"Give me a simple purple glow decoration plan"}
```

## Response

```json
{"answer":"..."}
```

## Safety notes

The current endpoint is a server foundation, not a finished in-game integration.
Do not expose the public endpoint before configuring rate limiting and monitoring.
The game must preview Auto Decor changes and obtain confirmation before deleting
or replacing any existing level objects. Never send level edits directly from an
unreviewed model response.
