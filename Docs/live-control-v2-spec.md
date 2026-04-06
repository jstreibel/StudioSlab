# LiveControl V2 Spec (Draft v0.1, Minimal Foundation)

## Purpose

Define the first V2 control/input subsystem to complement `LiveData V2`.

`LiveData V2` is for observing evolving runtime state.  
`LiveControl V2` is for supplying evolving runtime inputs.

This spec is intentionally narrow and implementation-oriented.

## Scope (Stage 0 / Stage 1)

In scope:
- control topics/streams (latest-value semantics)
- control source kinds (initial minimal set)
- time-domain tagging for control values/events
- typed control values (small set)
- control bindings (contract-level)

Out of scope (for this first slice):
- full sequencer integration
- persistent control recording format
- arbitrary expression/scripting language
- network transport

## Core Concepts

### `ControlSource`
A producer of control values/events.

Examples:
- mouse position / button / wheel
- UI sliders/knobs
- scripted source (later)
- recorded playback source (later)

### `LiveControlStream`
A live latest-value control stream (or topic) published by a source.

Examples:
- `labv2/input/mouse/world-pos`
- `labv2/input/mouse/button-left`
- `study/<id>/control/forcing-amplitude`

### `ControlBinding`
A typed connection from a control stream to a target input on a model/transform/view.

Examples:
- mouse position -> forcing center
- slider -> coupling strength
- button -> source enable/pulse

## Symmetry with `LiveData V2`

Desired architectural symmetry:
- `LiveData V2`: observation (state/telemetry/status)
- `LiveControl V2`: actuation/input (commands/values)

Both should support:
- named topics/streams
- typed access
- explicit timing semantics

## Time Domains (Control)

Every control value/event should declare a time domain.

Initial supported domains:
- `WallClockTime` (first implementation target)
- `SimulationTime` (for replay/sequence integration later)
- `StepIndex` (later; deterministic discrete control application)

## Semantics: Level vs Event

Live controls should distinguish:

### `Level` (stateful/latest-value)
Examples:
- current mouse position
- current slider value
- current source amplitude

Latest-value semantics are appropriate.

### `Event` (edge/impulse)
Examples:
- button pressed/released
- trigger pulse

These may need event semantics rather than latest-only state.

For the first implementation slice, a simple event topic with latest timestamped event is acceptable.

## Minimal Typed Control Values (Stage 1)

Start with a small set:
- `bool`
- `DevFloat` (scalar)
- `Real2D` / `FPoint2D` (2D position)
- optional small structs (e.g. `MousePointerState`)

Avoid over-generalized variant systems initially.

## Control Binding Semantics (Contract)

A control binding must define:
- source stream/topic
- target endpoint
- type contract
- adaptation/mapping rule (if needed)
- application semantics:
  - continuous (sample latest on each update)
  - edge-triggered (consume event)

Examples:
- `MousePos2D (wall-clock)` -> `KG2D forcing center (world coords)`
- `MouseLeftDown (event)` -> `forcing enabled`
- `Slider amplitude (level)` -> `forcing coefficient`

## Preferred Control Injection Pattern (Important)

To avoid coupling UI/control logic into numerical dynamics:

- The **integrable function / solver dynamics** remains responsible only for stepping and reading its configured dependencies.
- Control consumers/bindings should update an **external source object** (or equivalent injected dependency/parameter object) that the dynamics already reads.
- The integrable function remains agnostic to where changes came from (mouse, UI, replay, network, script).

### Preferred flow
1. `LiveControl V2` source publishes control values/events
2. `ControlBinding` reads control state/events
3. `ControlBinding` mutates the target external source / parameter object
4. Solver/integrable function reads that object during normal stepping

### Benefits
- preserves separation of concerns
- avoids embedding input-system logic into model dynamics
- supports replay by swapping control sources without changing the solver
- keeps the same control target usable from UI, scripts, or network sources

## Threading / Safety Note (Control Injection)

The dynamics may read the external source while control bindings write to it.

Therefore, the **external source object (or its parameter state)** must provide a thread-safe access policy where needed.

Possible implementation choices (case-dependent):
- atomics for small scalar flags/values
- small mutex around source parameters
- double-buffered parameter structs swapped at safe points

The integrable function can remain agnostic, but the injected source implementation must be safe.

## First Target Use Case (Driving Requirement)

Interactive `KG R^2->R` forcing:
- user moves mouse over a field view
- mouse position controls forcing center
- button/slider controls amplitude
- model consumes control values while running

This should be implemented with best-effort wall-clock control first.

## Relation to Reproducibility

Interactive wall-clock control is not fully reproducible by itself.

Reproducible replay requires:
- recording control events/levels with timestamps and time domain
- declared mapping policy to simulation time / step index (future)

This first spec only defines live control semantics, not replay.

## Suggested Implementation Staging

### Stage 1 (minimal)
- `LiveControl V2` topics/hub (latest-value)
- mouse/UI control sources
- typed reads/writes for small value types
- one model binding (`KG R^2->R` forcing)

### Stage 2
- event recording with timestamps/time domains
- playback source

### Stage 3
- sequence integration and deterministic replay modes

## Open Questions (deliberately deferred)

- exact topic class hierarchy vs templates
- event queue semantics vs latest-event semantics
- per-source threading model
- networked control sources
