# V2 Iteration Baseline Sprint

## Goal
- Establish a low-entropy baseline before high-velocity StudioSlabV2 feature/UX iteration.
- Keep upstream ImGui migration stable while V2 work grows.

## Scope (Short Sprint)
- Duration: 1-2 days.
- Focus: guardrails, not feature expansion.

## Work Items

### S1) Quality Gate Adoption
- Use `Scripts/quality-gate.sh` as the standard pre-merge gate.
- Extended local validation (when touching GUI paths):
  - `Scripts/quality-gate.sh --with-runtime`
- Deliverable:
  - gate command referenced in team workflow and PR checklist.

### S2) Blueprint Surface Ownership Cleanup
- Keep only `Studios/Lab/Blueprints` as the active Blueprint code path.
- Prototype Blueprint mirror is removed to avoid drift.
- Deliverable:
  - no `Studios/Prototype/Blueprints` sources remaining.

### S3) Test Discoverability Repair
- Ensure CTest discovers at least one stable smoke/unit target.
- Add one minimal always-on CTest registration path for CI sanity.
- Deliverable:
  - `ctest --test-dir <build-dir>` reports discovered tests (not zero).

### S4) Migration Doc Hygiene
- Keep these files in sync with implementation:
  - `Docs/imgui-migration-checklist.md`
  - `Docs/validation/imgui-blueprints-smoke.md`
  - `Docs/validation/imgui-upstream-parity-log.md`
- Deliverable:
  - latest validation run recorded with command and timestamp.

## Exit Criteria
- `Scripts/quality-gate.sh` passes.
- Upstream default build/profile is green.
- Legacy fallback profile still compiles (deprecated path).
- Runtime smoke matrix green for upstream-default and legacy-flag profiles.
- CTest discovers non-zero tests.

## Out of Scope
- New physics model features.
- Major UI redesign.
- Node-editor architecture rewrite.
