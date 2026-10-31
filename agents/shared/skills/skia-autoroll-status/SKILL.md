---
name: skia-autoroll-status
description: >-
  Inspect and query the status of Skia AutoRollers. Use when asked about dependency
  autoroll status, in-flight rolls, recent roll failures, or unrolled revisions.
---

# Skia AutoRoller Status Inspection

The Skia AutoRoll service (https://autoroll.skia.org) manages automatic
dependency roll CLs into parent projects like Chromium.

The web UI uses dynamic web components, so query the Twirp RPC API directly
instead of scraping HTML pages.

## API Endpoint

- **URL**:
  `https://autoroll.skia.org/twirp/autoroll.rpc.AutoRollService/GetStatus`
- **Method**: `POST`
- **Header**: `Content-Type: application/json`
- **Payload**: `{"roller_id": "<roller_id>"}` (e.g.,
  `{"roller_id": "perfetto-chromium-autoroll"}`)

## Example Query

Query the status of a roller (e.g., `perfetto-chromium-autoroll`):

```bash
curl -s -X POST "https://autoroll.skia.org/twirp/autoroll.rpc.AutoRollService/GetStatus" \
  -H "Content-Type: application/json" \
  -d '{"roller_id": "perfetto-chromium-autoroll"}'
```

## Interpreting Response Fields

- `status.mode.mode`: Roller operating mode (`RUNNING`, `STOPPED`, `DRY_RUN`,
  `OFFLINE`).
- `status.status`: Current state (e.g., `idle`, `in progress`,
  `success throttled`, `stopped`).
- `status.current_roll`: Details of the in-flight roll CL (try jobs, commit
  ranges, Gerrit issue ID), or `null` if none.
- `status.not_rolled_revisions`: Array of pending commits waiting to be rolled
  into the parent repository.
- `status.recent_rolls`: History of completed/attempted rolls.
  - `result`: `SUCCESS` or `FAILURE`.
  - `try_jobs`: List of CQ / trybot runs with names and buildbucket URLs.
- `status.issue_url_base`: Base URL to construct Gerrit CL links
  (`https://chromium-review.googlesource.com/c/<id>`).
