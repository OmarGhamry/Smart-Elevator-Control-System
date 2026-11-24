# Contributing

Thanks for contributing to the Smart Elevator Control System.

## Branch Policy

- `main` is the protected stable branch. Only the repository owner (`@OmarGhamry`) is allowed to push to it or merge into it.
- `develop` is the shared integration branch.
- Contributors must not commit directly to `develop` or `main`.
- All team changes must be submitted through a Pull Request targeting `develop`.
- Pull Requests targeting `main` are reserved for the repository owner when promoting a stable release from `develop`.
- Do not force-push to protected branches or delete them.

## Required Workflow

1. Start from the latest `develop` branch:
   ```bash
   git switch develop
   git pull origin develop
   ```
2. Create a descriptive task branch:
   ```bash
   git switch -c feature/floor-detection
   ```
   Other examples:
   - `feature/request-handling`
   - `fix/floor-sensor-debounce`
   - `docs/update-requirements`
3. Work only on the files related to your task.
4. Commit with a short, meaningful message.
5. Push your task branch:
   ```bash
   git push -u origin feature/floor-detection
   ```
6. Open a Pull Request with:
   - **Base:** `develop`
   - **Compare:** your task branch
7. Describe the change, tests, hardware checks, and any known limitations.
8. Wait for review and approval before merging. The repository owner/integration lead performs the merge into `develop`.

## Stable Releases

Only the repository owner promotes a tested and stable `develop` state into `main`, using a Pull Request from `develop` to `main` or another owner-controlled release process.

## Commit Message Examples

- `Add floor request handling`
- `Implement floor sensor validation`
- `Update system architecture diagram`
- `Fix floor sensor state transition`

## Pull Request Checklist

- [ ] Base branch is `develop`.
- [ ] The change has a clear description.
- [ ] Related tests or hardware checks have been completed.
- [ ] Documentation has been updated when needed.
- [ ] No unrelated files, credentials, or temporary output are included.
- [ ] The change does not introduce an unapproved scope change.

## General Guidelines

- Avoid committing generated files, temporary files, or IDE-specific user settings.
- Do not overwrite another teammate's work without discussing it first.
- Keep functions and modules focused.
- Document important design decisions and assumptions.
- Add or update tests when changing system behavior.
