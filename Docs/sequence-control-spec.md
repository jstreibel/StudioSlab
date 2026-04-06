# Sequence and Control Spec (Draft v0.1)

## Purpose

Define the timing and control model for authored interaction/presentation in StudioSlab.

This spec is a bridge to:
- interactive open-ended simulations
- simulation-time camera choreography
- recorded/replayed inputs
- reproducible studies

## Scope

In scope:
- time domains
- sequence tracks
- control sources
- playback/replay semantics
- determinism profiles (high level)

Out of scope:
- final UI implementation of a sequencer editor
- full file format
- symbolic control expressions language

## Core Concepts

### `Sequence`
A container of time-based tracks and markers.

A Sequence can target:
- cameras
- views (visibility/focus)
- annotations
- controls
- parameter automation
- capture/export actions (future)

### `Track`
A typed timeline channel inside a sequence.

Examples:
- camera transform track
- annotation visibility track
- scalar parameter automation track
- control playback track

### `ControlSource`
A source of input signals.

Kinds (initial conceptual set):
- live interactive (mouse, keyboard, UI)
- scripted/procedural
- recorded track playback
- remote/network input

### `Binding`
A typed connection from control/track output to a model/transform/view target input.

## Time Domains (Must Be Explicit)

Every Sequence and Track must declare its time domain.

Supported domains (initial target):
- `SimulationTime`
- `StepIndex`
- `WallClockTime`
- `SequenceTime`

Notes:
- `WallClockTime` is best-effort and non-deterministic
- `SimulationTime` / `StepIndex` are preferred for reproducible authored behavior

## Sequence Semantics

### Authoring-Time Behavior
- Tracks are edited in their declared domain
- Keyframes/segments/markers are interpreted in that domain
- Cross-domain preview requires explicit mapping (if supported)

### Runtime Behavior
- Sequence evaluation consumes a clock provider for its domain
- Evaluated outputs are emitted as control/value events
- Bindings route those outputs to targets

## Control Semantics

### Live Controls (interactive)
- Driven by user input/UI/network in wall-clock time
- Suitable for exploration and open-ended runs
- May be recorded for replay

### Recorded Controls
- Time-stamped sequence/control events captured from live sessions
- Replayable against runs or views
- Must preserve time domain metadata

### Scripted Controls
- Procedural or expression-based control sources
- Can be deterministic if driven by deterministic clocks and parameters

## Replay and Reproducibility

### Determinism Profiles (Conceptual)
- `InteractivePreview`
  - prioritize responsiveness
  - may use best-effort update paths

- `DeterministicLocal`
  - fixed seeds/settings/order
  - reproducible on same backend/profile

- `ReferenceExact` (future)
  - stricter precision/determinism guarantees

### Replay Rule
To claim reproducibility, replay must specify:
- run specification
- determinism profile
- recorded controls/sequence inputs
- time domain and mapping policy

## Target Use Case (Driving Requirement)

Interactive `Klein-Gordon` field with external forcing `J(x^\\mu)`:
- open-ended run
- mouse drives forcing source/control
- field monitor updates live
- camera/annotations can be tied to simulation time
- interaction can be recorded and replayed

This scenario should be treated as a primary acceptance target for the sequence/control architecture.

## Interaction with Current V2 Architecture

Current foundations already support:
- open-ended V2 runs (`FNumericTaskV2`)
- passive live monitoring (`LiveData V2`, session topics)
- `LabV2` launch and observability surfaces

Missing for this spec:
- `LiveControl V2` (input/control publication and routing)
- typed control bindings into models/transforms
- sequence evaluation runtime
- control recording/replay infrastructure

## Implementation Staging (Recommended)

### Stage 1: LiveControl Foundation
- minimal control topics/streams (sibling to LiveData)
- wall-clock interactive inputs
- typed bindings into selected V2 model inputs

### Stage 2: Recording
- capture control events with time domain metadata
- basic playback into running models (wall-clock or simulation-time)

### Stage 3: Sequencer Core
- sequence object + track evaluation
- camera and annotation tracks first
- control playback tracks next

### Stage 4: Reproducible Study Playback
- bind sequence + run specification + assets into a Study replay flow

## Non-Goals (for early versions)

- arbitrary TeX as executable control/math language
- full cinematic editor parity with Unreal
- universal cross-domain interpolation from day one

