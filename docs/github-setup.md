# GitHub Branch Protection Setup

The repository workflow is:

- `develop`: default branch and team integration branch.
- `main`: stable/release branch controlled only by `@OmarGhamry`.
- Contributors work on task branches created from `develop` and open Pull Requests into `develop`.

## One-time repository settings (owner action)

The GitHub connector used for repository file/branch operations cannot change repository-level branch protection or the default-branch setting. Complete these settings in GitHub while signed in as `@OmarGhamry`.

### 1. Set `develop` as the default branch

1. Open **Settings → General → Default branch**.
2. Select `develop`.
3. Confirm the change.

This makes new Pull Requests default to `develop`. Contributors must still check the PR **base** before creating it.

### 2. Protect `develop`

Open **Settings → Branches → Add branch protection rule** (or create an equivalent repository ruleset) for the branch name pattern `develop`.

Enable:
- Require a pull request before merging.
- Require conversation resolution before merging (recommended).
- Block force pushes.
- Prevent branch deletion.

Do not grant contributors direct-push permission to this branch. The team submits changes through Pull Requests.

### 3. Protect `main` — owner only

Add a branch protection rule (or ruleset) for the branch name pattern `main`.

Enable:
- Require a pull request before merging.
- Restrict who can push to matching branches; add only `@OmarGhamry`.
- Block force pushes.
- Prevent branch deletion.

If the UI offers an administrator-bypass option, configure it so the restrictions are not bypassed by other administrators. Keep the owner as the only permitted pusher/bypass actor. The owner can promote a stable change from `develop` to `main` through an owner-managed Pull Request.

### 4. Verify the workflow

- A contributor's PR from `feature/...` should target `develop`.
- A contributor must not be able to push directly to `develop` or `main`.
- Only `@OmarGhamry` should have write/push permission to `main`.
- A release PR from `develop` to `main` is created and merged by the owner.

## Important limitation

Branch protection controls writes and merges; it does not hide a public branch. Since this repository is public, contributors and visitors can still view and fetch `main`. The policy prevents them from changing it, not from reading it.
