# Gerrit CLI Workflows (Public)

## 1. Change List Review Workflow

Execute these steps in chronological order to review a Change List (CL) when
provided with a link or ID:

1. **Query CL Metadata**: Retrieve CL details and verify its current status:
   ```bash
   vpython3 agents/shared/skills/gerrit-cli/scripts/gerrit_client_wrapper.py \
     changes \
     --host https://chromium-review.googlesource.com \
     --query "change:<id>"
   ```
2. **Fetch File Contents**: Retrieve the content of specific files modified in
   the CL:
   ```bash
   vpython3 agents/shared/skills/gerrit-cli/scripts/gerrit_client_wrapper.py \
     content \
     --host https://chromium-review.googlesource.com \
     --project <project> \
     --change <change_id> --revision current --path <file_path>
   ```
3. **Analyze Local Modifications**: Inspect the fetched contents and local diffs
   for any logic or formatting issues.
4. **Add Draft Review Comment**: Create a draft comment containing the review
   findings and ask the user to review and post it in Gerrit (never publish
   comments directly with `addpatchsetcomment` if non-author humans are
   involved):
   ```bash
   vpython3 agents/shared/skills/gerrit-cli/scripts/gerrit_client_wrapper.py \
     rawapi \
     --host https://chromium-review.googlesource.com \
     --path "/changes/<change_id>/revisions/current/drafts" \
     --method PUT \
     --accept_status 200,201 \
     --body '{"path": "/PATCHSET_LEVEL", "message": "Review findings: ..."}'
   ```

## 2. Shepherding & Submission Workflow

Execute these steps in chronological order to approve and merge a Change List:

1. **Check Related Changes**: Retrieve the status of all related changes to
   ensure there are no unresolved blockers:
   ```bash
   vpython3 agents/shared/skills/gerrit-cli/scripts/gerrit_client_wrapper.py \
     relatedchanges \
     --host https://chromium-review.googlesource.com \
     --change <change_id> --revision current
   ```
2. **Vote on Review Labels**: Apply the required Code-Review approval vote:
   ```bash
   vpython3 agents/shared/skills/gerrit-cli/scripts/gerrit_client_wrapper.py \
     setlabel \
     --host https://chromium-review.googlesource.com \
     --change <change_id> \
     --label Code-Review 1
   ```
3. **Submit Change**: Merge the change once all CQ presubmits pass and all
   approval requirements are satisfied:
   ```bash
   vpython3 agents/shared/skills/gerrit-cli/scripts/gerrit_client_wrapper.py \
     submitchange \
     --host https://chromium-review.googlesource.com \
     --change <change_id>
   ```
