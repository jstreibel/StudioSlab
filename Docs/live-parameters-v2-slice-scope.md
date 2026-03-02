# Live Parameters V2 Slice Scope (Draft v0.1)

## Purpose

Define one implementation-ready slice for runtime-adjustable simulation parameters, without bloating the V2 control architecture.

This doc is intentionally execution-oriented (what to build first), not a broad refactor manifesto.

## Problem Statement

Current state:
- V2 already has control transport (`LiveControlHubV2` + topics).
- Some simulations expose launch-time parameters.
- Runtime live control is model-specific today (first prototype exists for KG2D forcing).

Missing:
- a generic way to mark parameters as runtime-mutable vs runtime-const
- a consistent binding path from control topics to model runtime values
- a hot-loop-safe read contract for steppers

## Scope (First Slice)

In scope:
1. parameter mutability/exposure metadata
2. generic scalar binding path (`bool`, `DevFloat`, optional `int`)
3. first adopters: XY and Ising (`Temperature`, `ExternalField`)
4. LabV2 minimal panel integration for those bound parameters
5. tests for binder behavior and runtime safety contract

Out of scope (for this slice):
- structural parameter live edits (`L`, `N`, `Steps`, geometry/layout-changing params)
- replay/recording semantics
- generalized non-scalar object bindings
- full migration of all model families

## Contract: Parameter Policy

Each exposed parameter should carry two explicit policies:
1. Mutability policy
   - `Const`: may be read at runtime, never changed during run
   - `RuntimeMutable`: may be changed while running
2. Exposure policy
   - `Hidden`
   - `ReadOnlyExposed`
   - `WritableExposed`

Default policy for V2 integrations:
- expose by default as `ReadOnlyExposed + Const`

## Contract: Hot-Loop Read Cost

Stepper/runtime reads must stay trivial:
- `Const` parameters: copied immutable values in stepper-owned runtime config
- `RuntimeMutable` scalars: atomic-backed reads (no blocking path in step loop)

No mutex acquisition inside per-site/per-particle inner loops.

## Injection Pattern

Preferred flow:
1. launcher/recipe builds parameter descriptors
2. recipe selects which parameters are opened (`RuntimeMutable`)
3. binder maps those to `LiveControl` topics
4. binder writes runtime parameter handles
5. stepper reads handles in normal stepping

This keeps model dynamics agnostic of UI source details.

## First Adopters

1. XY Metropolis:
- open: `Temperature`, `ExternalField`
- keep const: `L`, `Steps`, `DeltaTheta` (for first slice)

2. Ising Metropolis:
- open: `Temperature`, `ExternalField`
- keep const: `L`, `Steps`

## Acceptance Criteria

1. XY/Ising run unchanged when no live parameter input is provided.
2. Changing `Temperature`/`ExternalField` during run affects subsequent sweeps.
3. No new blocking on simulation thread attributable to parameter reads.
4. LabV2 can list bound live parameters and current values.
5. Unit/integration tests cover:
- metadata policy defaults
- topic-to-parameter updates
- const parameters rejecting runtime writes

## Complexity

- First slice (framework + XY + Ising + tests): medium.
- Expected effort: a few focused implementation days.

## Related Docs

- `Docs/live-control-v2-spec.md`
- `Docs/index-v2-runtime.md`
- `Docs/v2-feature-backlog.md`
- `Docs/status-v2.md`
