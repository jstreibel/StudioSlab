# Slab Root Migration Policy (V2 Boundary Refactor)

## Purpose

This document defines how new platform/framework architecture is migrated into the root-level `Slab/` folder while legacy code under `Lib/` and apps under `Studios/` continue to work.

## Why `Slab/`

- `Slab` is the platform/tooling side of the project identity.
- `Lab/` would be too easy to confuse with `Lib/` in paths and diffs.
- `Studios/` remains the app/study side.

Conceptually:
- `Slab` = platform / runtime / tooling / shared monitoring infrastructure
- `Studios` = particular studies and executables

## Scope (What belongs in `Slab/` first)

Move shared **boundary/framework** code first:
- numerics runtime orchestration helpers
- live data publication infrastructure
- shared monitor host/runtime helpers
- reusable monitor windows and studios-side V2 slice scaffolding

Keep model/domain implementations in place for now:
- `Lib/Models/...` (including model-local `V2` recipes)

## Migration Rules

1. No namespace churn during physical moves
- File relocation and namespace refactors are separate waves.
- Keep existing namespaces (e.g. `Slab::Math::...::V2`) unless explicitly planned.

2. Move by boundary waves
- Prefer small, coherent moves (one subtree/module at a time).
- Build and smoke-test after each wave.

3. Preserve behavior
- Relocation waves should not intentionally change runtime behavior.
- Any bug fixes should be isolated and called out explicitly.

4. Avoid partial dependency inversion
- Do not move model code into `Slab/` just because it depends on V2 runtime pieces.
- `Slab/` is for shared framework/platform code, not all new code.

5. Prefer updating call sites over broad forwarding shims
- Temporary forwarding headers are allowed only if churn/risk is high.
- If few call sites exist, update includes/CMake directly.

## Current Migration Plan (Phased)

### Phase S1 (safe start)
- Create root `Slab/`
- Move `Studios/Common` shared V2 helpers to `Slab/Studios/Common`
- Update `Studios/CLI` and `Studios/Tests` includes/CMake references

### Phase S2
- Move shared Data V2 boundary code (live topics/hub) into `Slab/`-scoped infrastructure folder
- Keep `FSessionLiveViewV2` compatibility behavior intact

### Phase S3
- Move Numerics V2 framework code (task/session/scheduler/listener infrastructure)
- Keep model V2 recipes under `Lib/Models/.../V2`

## Validation Requirements per Migration Wave

- `cmake --build cmake-build-debug --target Studios SlabTests testsuite -j8`
- Relevant smoke runs:
  - `./Build/bin/Studios list`
  - `./Build/bin/Studios spi --help`
  - `./Build/bin/Studios rtor --help`
- For graphics-related moves (display-enabled machine):
  - `./Build/bin/SlabTests panel-plot-and-gui --seconds 5`
  - `./Build/bin/Studios spi --gl ...` or `rtor --gl ...`

## Stop Conditions (Re-evaluate Before Continuing)

Pause migration if any wave starts requiring:
- broad namespace changes
- large CMake target graph rewiring
- mixed behavioral fixes and relocations
- duplication/forwarders that hide ownership boundaries

At that point, write the next migration slice spec before proceeding.
