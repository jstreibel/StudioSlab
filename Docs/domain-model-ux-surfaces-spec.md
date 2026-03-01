# Domain Model and Editor Tool Surfaces (UI Modules) Spec (Draft v0.1)

## Purpose

Define the core domain entities and the editor tool surfaces (UI modules, `LabV2` direction) that expose them.

This document is the bridge between:
- product vision (`Docs/product-direction.md`)
- low-level runtime/data specs
- editor implementation decisions

## Scope

In scope:
- canonical entities and relations
- primary workflows
- editor tool surfaces (UI modules)
- mapping to current V2 architecture

Out of scope:
- detailed rendering implementation
- exact persistence formats
- full blueprint execution semantics
- low-level solver internals

## Terminology Clarifications (This Document)

- **Direction** (for example, "`LabV2` direction") means the current architectural trajectory / intended evolution path.
  - It is not a claim that the final implementation path is fully known.
- **Lab** means the conceptual editor/workbench role in the product architecture (scientific control plane / authoring environment).
- **`Studios/Lab`** refers to the legacy implementation of that workbench role.
- **`LabV2`** refers to the current new implementation path (`Studios/LabV2`, `StudioSlabV2`) for the future `Lab` concept.

## Core Entity Graph (Conceptual)

- `Study` contains `Assets`, `Models`, `Transforms`, `Views`, `Sequences`, `Bindings`
- `Transforms` create `Runs`
- `Runs` produce `LiveStreams` and `Datasets`
- `Views` consume `LiveStreams` and `Datasets`
- `ControlSources` affect `Runs`/`Transforms` via `Bindings`
- `BlueprintGraph` authors and organizes the above relationships

## Entity Responsibilities

### `Study`
- Authored container and presentation unit
- Owns layout, references, bindings, and narrative metadata
- Can be loaded, edited, replayed, and exported

### `Model` (physical-model centered)
- Encodes the physical model semantics first:
  - state type(s) / degrees of freedom
  - physical variables / fields
  - Hamiltonian / Lagrangian / energy / action (as applicable)
  - admissible inputs (initial/boundary/control) and observables
- Provides the semantic foundation that numerical realizations and runs act on

### `Transform`
- A generic data operation (broader than simulation)
- Includes:
  - model execution/simulation
  - analysis
  - filtering
  - projection/resampling
  - minimization / optimization steps
- Defines inputs, outputs, and execution semantics for a concrete operation
- Can be launched by the user or triggered by a blueprint graph

### `Run`
- Runtime instance with status, telemetry, progress, and lifecycle
- Emits live streams and optional datasets

### `Dataset`
- Stable/persistable output or imported input
- Reusable across studies and runs

### `LiveStream`
- Pollable live state/telemetry/control endpoint
- Latest-value semantics by default

### `View`
- Passive consumer of data
- Presents data (plot/render/table/diagnostics)
- Should be reusable and composable

### `ControlSource`
- Produces live or scripted input
- May be interactive, recorded, or remote

### `Binding`
- A declared **typed connection** between a source endpoint and a target endpoint
- Carries connection semantics, not just a pointer/reference
- Should specify (conceptually):
  - source endpoint
  - target endpoint
  - type contract (what flows)
  - adaptation rules (if needed: resample/project/convert/copy-vs-lease)
  - timing/update semantics (one-shot, live, sampled, control-driven)
- Examples:
  - `Dataset -> InitialCondition`
  - `LiveStream -> View`
  - `ControlSource -> external forcing term`
  - `Transform output -> next Transform input`

## Editor Tool Surfaces (UI Modules, Lab/Editor Context)

In this document:
- **Lab/Editor context** means the Lab-side authoring/control environment, currently implemented through the `LabV2` direction (not numerics runtime or CLI apps).
- **Editor tool surface (UI module)** means a user-facing functional area (for example: Run Manager, Data Browser, Launcher, Viewports).
- It does **not** mean a graphics/render surface.

These should become stable editor tool surfaces (UI modules) over time.

### 1. `Content Browser`
Purpose:
- browse reusable assets and study resources

Examples:
- datasets, snapshots, colormaps, shaders, sequences, blueprints

### 2. `Data Browser`
Purpose:
- inspect live streams and datasets
- show type/status/provenance

Current seed:
- `LabV2` Live Data V2 panel

### 3. `Run Manager`
Purpose:
- launch/stop/monitor runs
- inspect status, progress, errors, logs, telemetry

Current seed:
- `LabV2` task panel + simulation launcher

### 4. `Viewports / Views`
Purpose:
- plot and field rendering
- diagnostics and tables
- multi-window or tiled layouts

Current seed:
- passive GL monitors (`SPI`, `RtoR`) in `Studios` and `LabV2`

### 5. `Inspector / Details`
Purpose:
- configure selected entity/view/binding
- inspect metadata and parameters

Current seed:
- launcher/monitor panels and plot GUI (partial/legacy)

### 6. `Launcher / Transform Palette`
Purpose:
- instantiate model/transform runs quickly
- expose presets and common execution modes

Current seed:
- `LabV2` SimulationManagerV2 launcher window

### 7. `Blueprint Editor` (future)
Purpose:
- graph-based orchestration of sources/transforms/views/controls

Status:
- legacy prototype exists; V2 integration deferred

### 8. `Sequence Editor` (future, high importance)
Purpose:
- authored timing for camera, annotations, visibility, automation

Will require explicit time-domain semantics.

### 9. `Math/Equation Panel` (future)
Purpose:
- display and author structured mathematical definitions
- LaTeX rendering for presentation/explanation
- later: structured symbolic input pipeline

## Primary Workflows

### A. Interactive Experiment
1. Select a model
2. Configure parameters
3. Launch run (headless or monitored)
4. Attach views
5. Bind controls (mouse/UI)
6. Observe and iterate

### B. Reusable Data Pipeline
1. Import/generate dataset
2. Apply transform(s)
3. Inspect results in views
4. Save datasets/assets
5. Reuse as future inputs

### C. Presentation Authoring
1. Create/choose study
2. Arrange views and annotations
3. Attach sequence tracks
4. Replay or render/export

## Current V2 Mapping (Implemented Pieces)

For current implementation snapshot precedence, use `Docs/status-v2.md`.

- `FNumericTaskV2` = `Run` executor
- `FSimulationRecipeV2` = numerical/runtime builder for a configured model/transform execution
- `FSimulationSessionV2` = run/session state
- `LiveData V2 topics/hub` = `LiveStream` foundation
- passive GL monitor windows = `View` implementations
- `LabV2` = early `Run Manager` + `Data Browser` + `Launcher`

## Current Gaps (Expected)

- `Study` object model not yet formalized in code
- `ControlSource` / `LiveControl` V2 foundation exists (topics/hub), but recording/replay and broader binding coverage are not implemented
- `Sequence` subsystem not implemented
- asset management and persistence model not unified
- blueprint integration not yet on V2 contracts

## Design Rules (Pragmatic)

1. New features should classify themselves:
- `Data Source`, `Transform`, `Run`, `View`, `ControlSource`, `Binding`, or editor tool

2. Avoid embedding scheduler/runtime semantics in views.

3. Avoid direct ad hoc coupling between model code and UI where a view or binding layer can hold the responsibility.

4. Prefer reusable editor tool surfaces (UI modules) over one-off launcher windows when a feature repeats.
