# StudioSlab SPEC (AI-Oriented, Spec-Driven Work)

## Purpose

This document defines the project-level specification for AI-assisted work in `StudioSlab`.
It is intended to reduce ambiguity, keep changes safe, and preserve architectural intent while the codebase evolves.

This is **not** a full rewrite spec. It is a **boundary-refactor spec** for a large, mixed-era C++ physics codebase.

## Project Summary

`StudioSlab` is a CMake-based C++23 workspace for physics simulations, numerical methods, visualization, and experiments.

Main repository areas:
- `Lib/`: shared libraries (math, numerics, graphics, models, utilities, 3rd-party modules/submodules)
- `Resources/`: images, fonts, shaders, etc.
- `Studios/`: applications / executables / experiments
- `Docs/`: design notes and local contracts

Primary library product:
- `Slab` (static library), consumed by studio executables.

## Problem Context

The codebase is powerful but heterogeneous:
- mixed naming/style conventions from different periods
- mixed architectural generations (legacy and newer code)
- some subsystems have blurred responsibilities (notably numerics output/monitoring and graphics host/composition)

The current strategy is to **stabilize legacy behavior**, while building **clean V2 subsystems in parallel**.

## Product Direction (High Level)

`StudioSlab` is evolving toward a **scientific creation environment**:
- `Slab` = engine/platform/runtime foundation
- `Lab` = editor/workbench (control plane)
- `Studios` = authored studies/examples/apps

Product direction is data-centric:
- users author **Studies** (not only runs)
- simulations are a class of transforms over mathematical data
- views/monitors are passive
- interaction and presentation are first-class concerns

See:
- `Docs/product-direction.md`
- `Docs/product-vocabulary.md`
- `Docs/domain-model-ux-surfaces-spec.md`
- `Docs/study-model-spec.md`
- `Docs/sequence-control-spec.md`

## Refactor Strategy (Global)

### Core Rule

Build new architecture in parallel under `V2` namespaces / folders where feasible, while:
- keeping legacy behavior operational
- avoiding risky big-bang rewrites
- deprecating legacy interfaces once V2 replacements are proven

### What is being refactored

Primarily **architectural boundaries**, not physics kernels:
- numerics runtime orchestration
- output/listener scheduling
- live data publication for monitoring
- monitor/graphics host composition

### What is *not* the current target

- rewriting all math kernels/solvers
- rewriting all model implementations
- removing legacy code prematurely

## Current Architectural Direction (V2)

### 1. Numerics Runtime V2 (Implemented baseline)

Key concepts:
- `FSimulationRecipeV2`: configuration + builders (blueprint)
- `FSimulationSessionV2`: mutable runtime instance of one run (single-run state)
- `FNumericTaskV2`: task/lifecycle orchestration
- `Trigger + Listener` model instead of monolithic output channels

Design intent:
- separate **when to emit** (trigger) from **what to do** (listener)
- keep task generic and recipe-driven
- support finite and open-ended runs

### 2. Session Synchronization Contract (Implemented baseline)

Zero-copy publication is the default.

To make that safe:
- session-owned mutable state is exposed via **read leases**
- one writer (simulation task), multiple readers (monitors/analyzers)
- synchronization is centralized at the **session boundary**

This supports:
- best-effort monitors (`try` read lease)
- consistent analysis readers (blocking read lease)

### 3. Live Data V2 (Implemented minimal generalized layer)

Current V2 live data direction:
- generic named live topics/hub (minimal)
- telemetry topics (copied metadata)
- session-view topics (zero-copy lease-backed access)

Compatibility path:
- `FSessionLiveViewV2` remains as a facade while broader live data V2 evolves

### 4. Graphics / Monitor Direction (Partially refactored)

Monitor architecture direction:
- monitors are **passive observers**
- numerics publishes data/telemetry
- monitor/UI consumes latest available data (best effort)

Graphics host improvements already in place:
- shared visual host loop for `Studios` and `SlabTests`
- render-pass propagation fixes for composite windows
- visual regression app (`SlabTests`)
- passive GL monitor flows proven in `Studios` for:
  - `spi`
  - `rtor` (KGRtoR plane waves)
- `LabV2` workbench shell with V2 launcher + LiveData V2 observability + in-app passive monitors

### 5. Studios V2 Integration Pattern (Implemented direction)

`Studios` CLI is being kept as the command-line entrypoint, while slice-specific orchestration moves into reusable modules.

Current direction:
- `Studios/CLI/main.cpp` handles parsing and subcommand dispatch
- `Slab/Studios/Common/Simulations/V2/*` owns slice-specific config finalization, recipe building, and execution flows
- `Slab/Studios/Common/Monitors/V2/*` owns passive monitor windows
- `Slab/Studios/Common/*` owns shared host/task/telemetry helpers

This keeps CLI logic readable while preventing monitor/runtime duplication across subcommands.

Migration note (active):
- shared Studios-side V2 infrastructure has started moving to root `Slab/Studios/Common` (physical layout only; no namespace churn in the same wave)

### 6. Broad V2 Adoption Strategy (Planned / active direction)

V2 architecture should be shaped by **both**:
- a north-star interactive/presentable study (e.g. interactive KG forcing)
- a coverage portfolio of migrated model families (KG, SPI, MD, Monte Carlo, minimization, etc.)

Reason:
- north-star alone can overfit the platform
- broad migration all at once creates too much entropy

Recommended approach:
- migrate representative model families incrementally as architecture probes
- use the north-star study to drive sequence/control/presentation requirements

## Legacy vs V2 Policy

### Legacy (V1)

Legacy systems remain valid for production use while V2 grows:
- old numerics/output paths
- legacy graphics monitor paths

Allowed work on legacy:
- correctness fixes
- safety hardening
- build fixes
- compatibility maintenance

Avoid:
- adding new architectural complexity to legacy abstractions
- extending deprecated output/monitor coupling patterns

### V2

New features at the boundary level should prefer V2:
- new runtime/task behaviors
- new monitor/listener functionality
- new live data publication patterns

## Coding and Naming Expectations

### Naming

Target naming style (forward-looking default):
- PascalCase with prefixes (Unreal-like), e.g.
  - classes: `FSomeClass`
  - enums: `ESomeEnum`
  - interfaces: `IThing`

Legacy naming remains in place unless touched by an approved normalization wave.

### Change style

- prefer incremental, isolated commits
- preserve behavior unless the change is explicitly a bug fix or spec change
- reduce hidden coupling (include leakage, transitive dependencies, shared mutable globals)

## Build and Test Requirements (for AI Changes)

Minimum validation for code changes:
- `cmake --build cmake-build-debug --target testsuite -j8`
- targeted `./Build/bin/testsuite "<relevant-tags>"`

For architecture or cross-module changes:
- `cmake --build cmake-build-debug -j8` (full build)

For visual/graphics changes (when applicable):
- `SlabTests` scenario(s), e.g.
  - `./Build/bin/SlabTests list`
  - `./Build/bin/SlabTests panel-plot-and-gui --seconds 5`
- local visual smoke on a display-enabled machine when relevant (this environment may be headless), e.g.
  - `./Build/bin/Studios spi --gl ...`
  - `./Build/bin/Studios rtor --gl ...`

## AI Work Protocol (Spec-Driven)

### Before changing code

AI should clarify:
- target subsystem (`Lib/Math`, `Lib/Graphics`, `Studios`, etc.)
- whether the change is legacy hardening vs V2 work
- behavioral invariants that must remain true

### While changing code

AI should:
- keep changes narrow and module-local when possible
- use tests to lock behavior before/after fixes
- prefer composition over adding new inheritance coupling
- avoid hidden global state in new code

### After changing code

AI should report:
- what changed
- why it changed
- validation performed
- residual risks / known limitations

## Active V2 Map (Status)

### Completed / working baseline

- Numerics Runtime V2 core
- Session-wide read lease locking
- Live data V2 minimal generalization (topics + hub)
- `Studios` CLI V2 subcommands (`spi`, `metropolis`)
- `Studios` CLI V2 subcommands (`spi`, `metropolis`, `rtor`)
- `Studios spi --gl` passive monitor prototype
- `Studios rtor --gl` passive monitor prototype (KGRtoR plane waves)
- `SlabTests` visual test runner
- graphics render-pass propagation fixes
- row/column pane framing and event routing fixes
- shared graphics visual host for `Studios` + `SlabTests`
- `Studios` V2 consolidation helpers:
  - task runner helpers (`NumericsV2TaskUtils`)
  - monitored GLFW runner (`V2SimulationRunners`)
  - passive monitor windows under `Slab/Studios/Common/Monitors/V2`
  - slice builders/execution flows under `Slab/Studios/Common/Simulations/V2`
- Numerics V2 real-time baseline:
  - wall-clock triggers
  - `LatestOnly` delivery (baseline implementation)
- Native V2 listeners (initial set):
  - cursor history
  - state snapshot (copied)
  - scalar time DFT

### Still expected (next waves)

- Data V2 expansion beyond session/telemetry topics
- monitor-oriented reusable passive UI components (beyond current SPI/KGRtoR windows)
- more V2 outputs/listeners (snapshots/history/DFT model-specific adapters/listeners)
- deeper graphics composition cleanup (`Row/Column/Panel` unification path)
- optional async/buffered delivery refinements and TCP-facing publishers
- additional `Studios` V2 simulation slices using the extracted `Common/Simulations/V2` pattern

## Key Architectural Invariants (Must Preserve)

1. **No data races for cross-thread state reads**
- best-effort monitors may skip frames
- they may not read unsafely

2. **Numerics task remains generic**
- task orchestrates runtime
- recipe builds session/configuration
- session owns mutable run state

3. **Scheduling and listening stay separated in V2**
- trigger = when
- listener = what

4. **Legacy and V2 can coexist**
- V2 introduction must not silently break legacy workflows

5. **Visual fixes must be backed by runnable scenarios**
- use `SlabTests` for graphics/monitor regressions

6. **`Studios` CLI should remain orchestration-focused**
- parsing/dispatch in `main.cpp`
- slice construction/execution in `Slab/Studios/Common/Simulations/V2`
- monitor rendering in `Slab/Studios/Common/Monitors/V2`

## Decision Guidance (When in Doubt)

Prefer the option that:
- keeps behavior explicit
- reduces hidden coupling
- keeps V2 APIs small and testable
- avoids importing legacy abstractions into V2

If a change requires broad coupling across numerics + data + graphics, split it into:
1. contract/spec step
2. minimal implementation slice
3. validation wave

## Related Local Docs

- `AGENTS.md` (repo-specific workflow and constraints)
- `Docs/graphics-composition-contract.md` (render/composition rules)
- `Docs/slab-root-migration-policy.md` (root `Slab/` migration rules)
- `Docs/product-direction.md` (product vision / direction)
- `Docs/product-vocabulary.md` (stable terminology)
- `Docs/domain-model-ux-surfaces-spec.md` (domain entities + editor surfaces)
- `Docs/study-model-spec.md` (Study object and reproducibility framing)
- `Docs/sequence-control-spec.md` (sequence/control/time-domain draft)
- `Docs/v2-model-coverage-matrix.md` (broad V2 migration planning matrix)
