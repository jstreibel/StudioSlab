# V2 Feature Backlog (Compact, Draft v0.1)

## Purpose

Planning-oriented task list for V2 work.

Use together with:
- `Docs/v2-model-coverage-matrix.md` (breadth/coverage planning)
- `Docs/study-model-spec.md` and `Docs/sequence-control-spec.md` (product/UX architecture)

## Status / Priority Conventions

- Status: `todo`, `active`, `blocked`, `done`
- Priority: `P0` (now), `P1` (next), `P2` (later)

## Current Recommended Milestone (`NS0`)

Goal: enable the first north-star interaction loop (field + monitor + live control).

### `P0` `LC-00` — `LiveControl V2` spec
- Define control topics/streams, source kinds, timing domains, and binding semantics.

### `P0` `KG2D-00` — `KG R^2->R` V2 slice scoping
- Pick exact legacy path and migration seam (likely `LeadingDelta`-related, but scoped).

### `P0` `KG2D-01` — `KG R^2->R` V2 headless slice (`V2-H`)
- `FSimulationRecipeV2` + `FNumericTaskV2` path, tests included.

### `P0` `KG2D-02` — `KG R^2->R` passive monitor (`V2-M`)
- Field view + telemetry via `LiveData V2`.

### `P0` `LC-01` — Minimal `LiveControl V2` implementation
- Wall-clock input stream(s), topic/hub basics, typed control values.

### `P0` `KG2D-03` — First control binding prototype (“poke field”)
- Mouse/UI control -> external forcing `J(x^\\mu)` (best-effort wall-clock path).

## Parallel Track (keep one item active max)

### `P1` `CVG-00` — Metropolis `V2-M`
- Add a monitored V2 path (telemetry/diagnostics view), not only headless.

## Follow-up Tracks (next)

### Study / Sequence
- `P1` `SEQ-00` — Sequence runtime spec refinement (tracks, clocks, domain mapping)
- `P1` `SEQ-01` — Minimal sequence runtime (camera + annotations)
- `P1` `ST-00` — In-memory `Study` object skeleton

### LabV2 Editor Tool Surfaces
- `P1` `LAB-00` — Data Browser polish (filtering, grouping, row details)
- `P1` `LAB-01` — View management surface (focus/close/retile)
- `P1` `LAB-02` — Run Manager improvements (grouping/error clarity)

### Coverage Portfolio
- `P1` `CVG-01` — MD V2 scoping
- `P1` `CVG-02` — MD `V2-H` -> `V2-M`
- `P1` `CVG-03` — XY/Ising V2 scoping
- `P2` `CVG-04` — Functional minimization V2 slice

## Working Rule (Entropy Control)

Keep at most:
- 1 north-star item active
- 1 coverage item active
- 1 editor/tooling item active

If ambiguity grows, stop and resolve spec/contracts before continuing implementation.

