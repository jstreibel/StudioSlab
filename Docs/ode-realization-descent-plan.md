# ODE Model -> Realization Descent Plan

## Purpose

Define the shortest safe descent from the current `Model V2` authoring surface into first `Realization` work.

This is not a general realization spec.
It is a narrow plan for the first ODE-oriented handoff.

## Status Note (`2026-03-14`)

- `RZ-00`, `RZ-01`, `RZ-02`, and the core `RZ-03` runtime bridge are now implemented in code.
- The remaining follow-up is integration work around numeric scalar bindings and a LabV2 launch path.
- Use `Docs/handoff-ode-realization-rz03.md` for restart context before touching that follow-up work.

## Use This When

- deciding whether `Model` is ready to feed a first realization slice
- selecting the next implementation step after `LAB-13`
- designing a realization contract for oscillator-like systems
- preventing premature descent into PDE/field/runtime generality

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `Docs/model-v2-semantic-environment.md`
3. this file
4. `Docs/index-v2-runtime.md`
5. `Docs/handoff-ode-realization-rz03.md`
6. `Docs/domain-model-ux-surfaces-spec.md`

## Code Map

- Model semantics / overview / assumptions:
  - `Slab/Core/Model/V2/ModelAuthoringV2.h`
- Model types / validation / type inference:
  - `Slab/Core/Model/V2/ModelTypesV2.h`
- Model notation parsing:
  - `Slab/Core/Model/V2/ModelNotationV2.h`
- Seed validation examples:
  - `Slab/Core/Model/V2/ModelSeedsV2.h`
  - `Lib/tests/test_model_v2.cpp`
- Current runtime core:
  - `Slab/Math/Numerics/V2/Runtime/SimulationRecipeV2.h`
  - `Slab/Math/Numerics/V2/Runtime/SimulationSessionV2.h`
  - `Slab/Math/Numerics/V2/Task/NumericTaskV2.h`

## 1. Working Decision

`Model V2` is stable enough for a narrow realization descent.

The descent should start with:
- finite-dimensional dynamical systems
- ODE-like relations
- explicit local definitions/relations only
- one realization-facing contract derived from canonical model semantics

The descent should not start with:
- PDE/field realization
- broad solver catalogs
- discretization stories
- boundary-condition-heavy workflows
- Framing/Sequence/Study integration

## 2. Why This Is Safe Now

Current `Model V2` already provides:
- ambient vocabulary and local semantic resolution
- explicit definitions and relations
- assumption/materialization workflow
- transactional editing and draft semantic delta
- semantic status/classification
- direct creation of local definitions and relations

Current known gaps that do not block first ODE descent:
- exact notation-span navigation
- richer inference heuristics
- user-authored base-vocabulary editing
- batch authoring / undo stack

## 3. Descent Target

The first realization slice should consume a canonical model and produce an ODE-oriented realization descriptor.

Think of that descriptor as:
- realization-ready summary
- not yet a full runtime/session/run object
- not yet a solver implementation menu

Minimum supported example:
- harmonic oscillator and close variants such as damped oscillator

Validation-only pressure example:
- Klein-Gordon remains useful for checking that the boundary is intentionally ODE-first
- it should not be the first realization target

## 4. Model -> Realization Contract

The first contract should expose only stable, realization-relevant semantics.

Required canonical inputs:
- model id / name
- semantic health/status
- inferred model class / character
- active base vocabulary preset
- time coordinate if present
- canonical state variables
- scalar parameters
- observable symbols
- canonical local relations

Realization boundary rule:
- realization reads the canonical, typed model subgraph, not an arbitrary plotted/exported neighborhood
- provenance/debug/view-only edges are outside the contract
- pending assumptions must be resolved or canonized before they can contribute to realization
- use `Docs/model-semantic-graph-layering.md` when changing this boundary

Required gate:
- no blocking semantic errors
- no unresolved symbols
- no pending assumptions that the realization path depends on

Required ODE-oriented derivables:
- state variable ids and notations
- parameter ids and notations
- ordered relation set selected for realization
- realization diagnostics when the model is not ODE-ready

Explicitly out of scope for this contract:
- solver choice
- discretization policy
- runtime control binding semantics
- persistence format
- field-space / PDE mesh semantics

## 5. Model Layer Addition That Was Needed Before Descent

This requirement is now implemented:
- explicit model-level initial-condition slots for canonical ODE state variables

The missing layer that had to be added before descent was:
- initial-condition slots for canonical state variables

Why this is the right addition:
- it is realization-facing
- it belongs to model meaning, not only runtime wiring
- it is much smaller than general control/boundary semantics

Do not broaden this yet into:
- general boundary conditions
- control-source bindings
- forcing/operator pipelines

## 6. Planned Steps

### `RZ-00` Contract Freeze

Goal:
- define one C++ realization-facing descriptor for ODE-ready models

Status:
- implemented

Output:
- contract doc plus code-level data structure

Rules:
- consume canonical semantic overview data
- do not re-infer meaning in a disconnected runtime layer
- reject models that are not ODE-ready with explicit diagnostics

### `RZ-01` ODE Descriptor Extraction

Goal:
- derive the descriptor from `Model V2`

Status:
- implemented

Minimum contents:
- time coordinate
- state variables
- parameters
- observables
- realization-selected relations
- readiness diagnostics

Validation:
- harmonic oscillator extracts cleanly
- damped oscillator with `\gamma` extracts cleanly once `\gamma` is explicit

### `RZ-02` Initial-Condition Semantics

Goal:
- add explicit initial-condition declarations for the ODE slice

Status:
- implemented

Rules:
- keep them model-level, not run-level
- support only the minimum needed for ODE state initialization
- avoid general boundary/control stories

Validation:
- oscillator can express initial state for canonical state variables

### `RZ-03` First Runtime Bridge

Goal:
- hand one ODE descriptor into a simple realization/runtime builder

Status:
- implemented

Rules:
- one narrow path
- one example family
- diagnostics first, solver breadth later

## 7. Exit Criteria For “Ready To Descend”

Start realization work once these are true:
- the contract in `RZ-00` is frozen
- ODE readiness is computable from canonical model semantics
- oscillator-class models extract deterministically
- one explicit initial-condition story exists

Do not wait for:
- perfect notation-span navigation
- ontology-deep classification
- full PDE/field semantics

## 8. Validation Ladder

### Phase A

- harmonic oscillator
- damped oscillator

Questions answered:
- can Model semantics identify the state basis cleanly?
- can realization gating reject unresolved/pending semantics cleanly?
- can the user see what is missing before runtime descent?

### Phase B

- use Klein-Gordon only as a negative/deferral check

Questions answered:
- does the ODE path refuse PDE-like models clearly?
- is the handoff boundary legible instead of silently wrong?

## 9. Risks To Avoid

- building a second semantic model inside runtime code
- letting runtime re-guess state/parameter roles independently
- mixing initial conditions with live control semantics too early
- broadening the first descent into generic PDE realization
- coupling realization progress to notation-span/editor polish

## 10. Recommended Next Action

The core descent ladder in this plan is now implemented.

Current follow-up:
- keep the bridge descriptor-driven
- add the smallest numeric scalar-binding story needed to launch it
- add one LabV2 launch path for oscillator-class models
