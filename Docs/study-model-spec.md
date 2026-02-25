# Study Model Spec (Draft v0.1)

## Purpose

Define the authored artifact model (`Study`) and its relation to runs, data, views, sequences, and reproducibility.

This document is intended to guide `LabV2` evolution and future persistence/export design.

## Key Decision

The user-facing authored output is a **Study**, not a `Model`.

- `Model` = scientific/computational model or transform component
- `Study` = authored scientific experience/project using models

This supports presentation, sequencing, and multi-model composition without overloading the meaning of “model”.

## Study Definition

A `Study` is an authored container that describes:
- what data is used
- what models/transforms exist
- how runs are configured and launched
- what views are shown
- how controls are bound
- how timing/presentation is orchestrated
- what outputs are preserved/exported

## Study Composition (Minimum)

A Study should be able to contain references to:
- `Assets`
- `Models` / `Transforms`
- `RunTemplates` (optional)
- `Datasets`
- `LiveStreamBindings`
- `Views` and layout
- `ControlSources`
- `Bindings`
- `Sequences`
- `Annotations`
- `Provenance`

## Study Runtime vs Study Asset

Separate:

- **Study Asset (authored spec)**
  - persistent authored description

- **Study Session (editor/runtime state)**
  - open windows
  - temporary runs
  - interactive edits not yet saved

This distinction will reduce editor complexity later.

## Run and Reproducibility

### `RunSpecification` (recommended)

A `RunSpecification` should capture enough declared information to reproduce a run under a declared determinism profile.

At minimum:
- model/transform identity
- model/runtime version identifiers (or hashes later)
- parameters
- input dataset references
- control bindings (if any)
- RNG seeds
- solver/runtime profile
- precision/determinism settings

### `RunRecord`

A completed or in-progress run record should include:
- status and lifecycle timestamps
- telemetry summary
- produced datasets/live stream identifiers
- errors/warnings
- provenance back-reference to `RunSpecification`

## Time Domains in Study Context

Studies must support multiple explicit time domains:
- `StepIndex`
- `SimulationTime`
- `WallClockTime`
- `SequenceTime`
- `FrameTime` (view/render pipeline)

Rules:
- every sequence track declares its domain
- every control recording declares its domain
- conversions between domains must be explicit (or unsupported)

## Views and Layout in a Study

Views are first-class study elements.

A view configuration should eventually include:
- view type (`plot`, `field`, `table`, etc.)
- data binding(s)
- visual style
- camera/region state
- interaction policy
- layout placement or docking metadata

Current state:
- `LabV2` windows and passive monitors are the seed of this system
- persistence of view layout/config is not yet implemented

## Control Bindings (Study-Level)

A Study should describe control intent, not only raw UI widgets.

Examples:
- mouse position -> external source center
- mouse button -> pulse amplitude gate
- slider -> coupling parameter
- recorded control track -> playback into run

Control bindings should target typed model/transform inputs.

## Sequences (Study-Level)

A Study may contain one or more sequences for:
- camera movement
- annotation timing
- view visibility/focus
- parameter automation
- control playback

Sequence semantics are defined in `Docs/sequence-control-spec.md`.

## North-Star + Coverage Strategy (Important)

### Recommendation
Use both:

1. **North-Star Study**
- a high-value interactive/presentable study that forces editor/runtime integration
- example target: interactive Klein-Gordon field forcing with simulation-time camera/annotations

2. **Coverage Portfolio**
- migrate multiple existing model families to V2 to validate broad architecture

### Why not only the north star
A single north-star can overfit architecture to one model family or interaction style.

### Why not migrate everything at once
Migrating all model families simultaneously creates too much entropy and makes spec feedback noisy.

### Suggested Coverage Portfolio (Representative)

Pick a rolling subset covering distinct execution/data patterns:
- `KGRtoR` (1D ODE/PDE-like field evolution)
- `KGR2toR` (2D field + interactive visual control pressure)
- `SPI` / stochastic path-integral (ODE/time-aware + field views)
- Molecular Dynamics (particle systems, thermostats, trajectories)
- Monte Carlo lattice models (Ising/XY; non-time or pseudo-time semantics)
- Minimization/optimization workflows (transforms producing converged states)

Use these as architectural probes, not simultaneous full feature migrations.

## Immediate Implementation Guidance

Before building Study persistence, stabilize:
- `LiveData V2` and future `LiveControl V2`
- `LabV2` launch/monitor/view surfaces
- sequence/control contracts (time domains and replay semantics)

Then introduce a minimal in-memory `Study` object model in `LabV2`.

