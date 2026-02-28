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

## Progress Notes (2026-02-27)

- `KG2D-01` done: `KGR2toR` baseline V2 headless slice (`Studios kg2d` + tests).
- `KG2D-02` done: passive GL monitor path for `KGR2toR` baseline V2.
- `LC-01` foundation done: `LiveControl V2` topics/hub + LabV2 visibility panel.
- `KG2D-03` done: first control binding prototype (LabV2 control source -> hub topics -> KG2D external forcing source).
- `KG2D-03.5` done: KG2D monitor-side control source publisher (GL monitor UI publishes control topics directly).
- `CVG-00` done: Metropolis V2 now has a passive monitor path (`Studios metropolis --gl`, LabV2 launcher).
- `LAB-00` done: Data Browser polish (topic filters, selected-topic details for LiveData/LiveControl).
- `LAB-01` done: View management surface baseline (list/select/focus/close/retile).
- `LAB-02` done: Run Manager improvements baseline (task filters, grouped counters, progress/cursor table, clear actions).
- `CVG-01` done: MD V2 scoping draft captured in `Docs/md-v2-slice-scope.md`.
- `CVG-02` done: MD baseline `V2-H` and `V2-M` paths now wired in CLI/LabV2 (`Studios moldyn`, `Studios moldyn --gl`).
- `CVG-03` done: XY and Ising V2 lattice slices now have `V2-H` + `V2-M` paths in CLI/LabV2 (`Studios xy|ising`, optional `--gl` passive monitors).
- `CVG-03.5` done: SlabTests visual smoke commands now include XY/Ising monitor paths (`xy-monitor-smoke`, `ising-monitor-smoke`).
- `CVG-04` active: Functional minimization V2 first-slice scope drafted in `Docs/functional-minimization-v2-slice-scope.md`.
- `LAB-03` done: LabV2 top-level panel/window orchestration centralized in `FLabV2WindowManager` with registry-driven panel surfaces.
- `LAB-04` done: ImGui-wrapped Slab canvas event routing hardened (mouse hit-test routing, keyboard focus routing, persisted bounds/last-mouse bridge).
- `LAB-05` done: Plot window controls moved to viewport-attached toolbar + attached detail inspector (`Docs/plot-window-ux-contract.md`).

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
- done (`LabV2` sidepane source + monitor-side source + CLI flags for KG2D).

## Parallel Track (keep one item active max)

### `P1` `CVG-00` — Metropolis `V2-M`
- Add a monitored V2 path (telemetry/diagnostics view), not only headless.

## Follow-up Tracks (next)

### Study / Sequence
- `P1` `SEQ-00` — Sequence runtime spec refinement (tracks, clocks, domain mapping)
- `P1` `SEQ-01` — Minimal sequence runtime (camera + annotations)
- `P1` `ST-00` — In-memory `Study` object skeleton

### LabV2 Editor Tool Surfaces
- `P1` `LAB-00` — Data Browser polish (filtering, grouping, row details) — done
- `P1` `LAB-01` — View management surface (focus/close/retile) — done
- `P1` `LAB-02` — Run Manager improvements (grouping/error clarity) — done
- `P1` `LAB-03` — Centralized panel/window orchestration contract — done
- `P1` `LAB-04` — Sub-window event routing contract and wrapper hardening — done
- `P1` `LAB-05` — Plot window control surface UX hardening — done

### Coverage Portfolio
- `P1` `CVG-01` — MD V2 scoping — done
- `P1` `CVG-02` — MD `V2-H` -> `V2-M` — done
- `P1` `CVG-03` — XY/Ising V2 lattice slices (`V2-H` -> `V2-M`) — done
- `P2` `CVG-04` — Functional minimization V2 slice

## Working Rule (Entropy Control)

Keep at most:
- 1 north-star item active
- 1 coverage item active
- 1 editor/tooling item active

If ambiguity grows, stop and resolve spec/contracts before continuing implementation.
