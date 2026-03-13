# StudioSlab Product Vocabulary (Draft v0.1)

## Purpose

This document fixes the core nouns used in product, UX, and architecture discussions.
It should be kept stable to reduce drift while V2 evolves.

## Canonical Terms

### `StudioSlab`
The overall product/workspace concept and codebase.

### `Slab`
The engine/platform/runtime/tooling foundation.

Examples:
- numerics runtime
- live data/control systems
- shared monitor/view infrastructure
- execution orchestration

### `Lab`
The editor/workbench experience (control plane).

Examples:
- launcher panels
- run manager
- data browser
- view layouts
- blueprint editor
- sequence editor

### `Studios`
Authored studies/examples/apps built using `Slab` and surfaced in `Lab`.

Can refer to:
- repository folder of executables/examples (`Studios/`)
- conceptual “studies” ecosystem

### `Study`
The primary authored user artifact (scientific experience/project deliverable).

A Study may contain:
- models/transforms
- runs
- datasets/streams
- views and layout
- annotations
- sequences
- interaction bindings

### `Model`
A scientific/computational model definition (or model-backed transform).

Examples:
- Klein-Gordon (R->R, R^2->R)
- SPI
- Molecular Dynamics
- Ising / XY Monte Carlo

`Model` is **not** the same as `Study`.

### `BaseVocabulary`
A readonly ambient semantic environment attached to a `Model`.

Examples:
- scalar sets such as `\\mathbb{R}`
- ambient operators such as `\\Box`
- notation conventions such as `\\dot{\\mathrm{state}}`

`BaseVocabulary` is not the same as local `Definitions`.
It supplies inherited semantic context that local definitions and relations may refer to, refine, or override.

### `NotationContext`
The authoring/projection layer used to parse and render notation against model semantics.

`NotationContext` is not a peer ontological layer beside `Definitions` and `Relations`.

### `Transform`
A data operation.

Includes:
- simulation/model execution
- analysis
- filtering
- projection
- resampling
- minimization steps

### `Run`
A concrete execution instance of a transform/model.

Has:
- lifecycle/status
- time cursor
- progress (optional for open-ended)
- telemetry
- outputs
- provenance

### `Dataset`
A stable data snapshot (persistable/reusable).

Examples:
- final field state
- history
- DFT output
- imported file-backed function

### `LiveStream`
An evolving latest-value data source.

Examples:
- live session state
- telemetry/status stream
- remote stream

Current V2 mapping:
- `LiveData V2 topics/hub`

### `View`
A passive visualization/inspection surface.

Examples:
- plot window
- field renderer
- diagnostics panel
- table
- LaTeX/math card

Views do not own simulation cadence.

### `ControlSource`
A live or scripted input source that can affect a run/transform.

Examples:
- mouse-driven external forcing `J(x^\mu)`
- UI sliders
- recorded control track
- network input

### `Binding`
A typed connection between entities.

Examples:
- dataset -> initial condition
- control source -> forcing term
- live stream -> view
- transform output -> next transform input

### `BlueprintGraph`
Graph-based authored composition of sources, transforms, views, controls, and bindings.

### `Sequence`
Time-based authored tracks controlling presentation and/or automation.

Examples:
- camera motion
- annotations
- view visibility
- parameter automation
- control playback

### `Asset`
Reusable authored or imported resource.

Examples:
- datasets
- functions
- colormaps
- shaders
- camera tracks
- equation cards
- blueprints

### `Connector`
Import/export/remote integration endpoint.

Examples:
- local file formats
- remote compute service
- dataset sync
- telemetry stream transport

## Time Vocabulary

Use explicit time-domain names:
- `StepIndex`
- `SimulationTime`
- `WallClockTime`
- `FrameTime`
- `SequenceTime`

Do not say just “time” in architecture/spec contexts without naming the domain.

## Reproducibility Vocabulary

### `RunSpecification` (recommended term)
The complete declared spec needed to reproduce a run, including:
- model identity/version
- parameters
- inputs/bindings
- solver/runtime profile
- seeds
- precision/determinism mode

Prefer this over saying “parameters alone” in strict technical contexts.

## Naming Guidance (Product Docs)

Prefer:
- “Study” for authored artifact
- “Model” for scientific system/transform component
- “Run” for execution instance

Avoid overloading:
- “Model” to mean a full authored presentation/project
