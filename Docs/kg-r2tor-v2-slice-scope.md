# KG R^2->R V2 Slice Scope (Draft v0.1)

## Purpose

Scope the first `Klein-Gordon R^2->R` migration wave into V2 without overcommitting architecture too early.

This is the likely next major architecture probe because it combines:
- 2D field visualization
- live monitoring
- eventual interactive forcing (`J(x^\\mu)`)
- strong alignment with the north-star study direction

## Candidate Legacy Inputs (Observed)

Relevant existing pieces:
- `Lib/Models/KleinGordon/R2toR/*` (equation state, solver, builder, outputs, generic GL monitor)
- `Studios/Fields/R2toR/LeadingDelta/*` (concrete builder + forcing-related boundary + custom monitor)

## Key Scoping Decision (Recommended)

Do **not** migrate the entire `LeadingDelta` workflow in one shot.

Instead split the migration:

### Phase A (`KG2D Core`)
- V2-capable `KG R^2->R` core recipe/session path (headless + monitored)
- generic/passive field monitor via `LiveData V2`
- no interactive control yet

### Phase B (`KG2D Control`)
- add minimal external forcing seam via `LiveControl V2`
- implement one control binding prototype (“poke field”)

This keeps the runtime/data/control responsibilities clear.

## Proposed First Slice (`KG2D-01` / `KG2D-02`)

### Goal
Produce `KG R^2->R` `V2-H` and `V2-M` with `Studios` and/or `LabV2` integration.

### Minimum deliverables
- `FSimulationRecipeV2` for a concrete `KG R^2->R` path
- `FNumericTaskV2` execution (finite first)
- `LiveData V2` telemetry/status/session publication
- passive field monitor window (heatmap/section as available)
- basic tests for headless execution and cursor/telemetry behavior

### What to defer
- interactive control input
- sequence integration
- advanced legacy outputs (history/snapshot pipeline parity)
- custom artistic monitor features

## Candidate Concrete Starting Paths

### Option 1 (Recommended): Simple generic `KG R^2->R` baseline
- Use `Lib/Models/KleinGordon/R2toR` solver/state/boundary foundation
- Start with simple/default initial/boundary conditions
- Establish V2 runtime + monitor path first

Pros:
- lower entropy
- cleaner V2 architecture seam
- less Studio-specific coupling

Cons:
- less immediately “wow” than `LeadingDelta`

### Option 2: `LeadingDelta`-derived path (scoped)
- Use `Studios/Fields/R2toR/LeadingDelta` as the source of a concrete scenario
- Port only what is needed to get a monitored V2 run

Pros:
- naturally leads toward interactive forcing use case
- richer visuals quickly

Cons:
- pulls Studio-specific legacy coupling into early migration
- risks mixing control concerns too early

## Recommendation

Start with **Option 1** for `V2-H/V2-M`, then add a **LeadingDelta-like forcing seam** once `LiveControl V2` exists.

This preserves a clean progression:
1. runtime + live data + passive view
2. live control
3. interactive forcing

## Initial Acceptance Criteria

### `KG2D-01` (headless)
- V2 run launches and completes via `FNumericTaskV2`
- tests cover basic progression and telemetry

### `KG2D-02` (monitored)
- passive field monitor renders from `LiveData V2`
- `Studios` and/or `LabV2` can launch it
- end-of-run behavior keeps final frame / telemetry semantics intact

## Known Architectural Pressure Points

These should be watched during migration:
- field monitor data handoff strategy (zero-copy vs copied display buffers)
- simulation-time semantics in 2D open-ended runs
- control injection seam location (model boundary vs recipe/session wrapper)
- view interaction mapping (screen coords -> field/world coords)

If any of these become unclear during implementation, stop and tighten the contract before broadening scope.

