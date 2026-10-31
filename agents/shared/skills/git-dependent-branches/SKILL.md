---
name: git-dependent-branches
description: >-
  Manage Git branches using depot_tools. Apply before any edit or commit, even
  without a branch request, to keep one logical change per branch. Prefer
  depot_tools branch commands over raw Git equivalents. If the workspace or
  request uses Jujutsu (`jj`), use a jj-specific skill instead.
---

# Git branch management with depot_tools

With depot_tools, a branch's upstream reference identifies what it depends on:
usually the configured trunk (usually `origin/main`) or another local branch for
a dependent branch. These links form a dependency graph. Related dependent
branches are often called a "stack" or "chain" of changes.

When uploading to Gerrit, squash mode produces one CL per branch, while
no-squash mode produces one CL per commit. In Gerrit, a "relation chain"
describes dependencies between uploaded CLs.

## Mandatory workflow

Run commands individually and check each result. Stop if leaving a detached HEAD
would make commits unreachable.

1. Inspect `git status`, `git branch --show-current`,
   `git rev-parse --abbrev-ref --symbolic-full-name '@{upstream}'`,
   `git log --oneline '@{upstream}..HEAD'`, `git upstream-diff`, `git diff`, and
   `git diff --cached`. Stop on unresolved state; use recovery below.
2. Choose the branch using the table. Judge commits and diffs, not just upstream
   configuration. Ask if the relationship to the task is unclear.
3. Preserve unrelated pending edits; ask before switching, without automatic
   stashing, committing, discarding, or carrying them over. Create and switch to
   the selected branch before any edit or commit, unless reusing it. Never edit
   or commit on trunk.
4. After every mutation, repeat step 1 and verify the expected branch, upstream,
   commits, and diff before continuing. For a new branch, confirm its name/base
   and that `git rev-parse HEAD '@{upstream}'` returns equal IDs.
5. On failure, cancellation, or unexpected state, stop. Use permitted read-only
   checks to inspect partial effects, report, and ask before retrying or
   mutating again. If recovery needs a prohibited operation, stop and explain
   the blocker; never bypass the restriction with another command.

| Situation                                       | Action                                         |
| ----------------------------------------------- | ---------------------------------------------- |
| Same logical change                             | Reuse the current non-trunk branch.            |
| Independent change or work on trunk             | `git new-branch <name>` from configured trunk. |
| Separate change requiring current unlanded work | `git new-branch --upstream-current <name>`.    |

Task order or a shared topic alone does not establish a dependency.

## Other operations

- Inspect the stack: `git map-branches`; follow branches' `@{upstream}`.
- Reparent: `git reparent-branch`. Rename: `git rename-branch`.
- Update: `git rebase-update` for all branches, or
  `git rebase-update --current --no-fetch --tree` for the current branch and
  descendants. Rebase dormant branches only when explicitly requested.
- Upload to Gerrit: `git cl upload`; prefer git-cl-helper if loaded.
- Insert/split: `git new-branch --inject-current <name>` creates and checks out
  a branch at the old upstream tip, then makes the original branch track it.
  This inserts a branch between the original branch and its upstream; it does
  not split or move commits. Plan the split first and stop before changing the
  graph if completion requires prohibited operations.

## Recovery

If upstream is missing or incorrectly tracks `origin/<branch_name>`, warn and
ask before fixing it with `git branch --set-upstream-to=<REF> <branch_name>`.
Choose the nearest ancestor ref: among candidates satisfying
`git merge-base --is-ancestor <candidate> <branch_name>`, minimize
`git rev-list --count <candidate>..<branch_name>`; fall back to configured
trunk. Rerun checkpoints and branch selection after each approved fix.
