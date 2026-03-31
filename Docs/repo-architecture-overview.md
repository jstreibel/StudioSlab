# Repository Architecture Overview

## Purpose

This document explains the monorepo split between reusable libraries, shared platform work, and executable targets.
Use it when the main question is "where should this code live?" rather than "how does this feature work?"

## Canonical Split

- `Lib/`
  - shared reusable libraries
  - legacy/core code still used across many targets
  - broad domain/model implementations (`Lib/Models`)
  - older graphics/runtime paths that still power many standalone studies
- `Slab/`
  - shared V2 platform/editor/runtime landing zone
  - root-level home for boundary refactors that should be reused by multiple targets
  - current examples include V2 numerics, live data/control, reflection, Plot2D V2, and shared monitor helpers
- `Studios/`
  - executable targets
  - standalone studies, CLI entrypoints, sandboxes, and workbench applications
  - includes both legacy apps and the current `LabV2` workbench target
- `StudioSlab`
  - the overall product/codebase name
  - in current builds, also the desktop workbench executable produced from `Studios/LabV2/`

## Current Project Shape

Two repo philosophies are active at the same time, and both are intentional:

1. A broad reusable toolbox for experiments and C++ studies.
- This is why `Lib/` and many `Studios/*` targets still matter.
- Standalone targets are not just leftovers; they are architectural probes and reusable experiments.

2. A main platform/workbench path.
- `StudioSlab` / `LabV2` is the current editor/workbench target.
- Shared V2 platform/editor/runtime code should increasingly move into `Slab/`.
- The workbench is desktop-first today; browser work is still a bounded platform spike.

## Placement Rules

Put code in `Slab/` when it is:
- shared platform/runtime/editor infrastructure
- part of an active V2 boundary refactor
- intended to be reused by multiple executables or multiple workbench surfaces

Keep code in `Lib/` when it is:
- reusable shared library code that already serves many targets
- model/domain implementation code
- legacy infrastructure that has not yet been explicitly migrated

Put code in `Studios/` when it is:
- a thin launcher, app, or sandbox
- target-specific orchestration
- an executable proving or stressing shared infrastructure

## Naming Disambiguation

- `Study`
  - the authored scientific artifact
- `Studios/`
  - the repository folder containing executable targets
- `Studio app`
  - one executable target under `Studios/`
- `StudioSlab`
  - the product/codebase name, and today also the `LabV2` desktop binary name
- `Slab`
  - the platform concept, the root migration folder, and the aggregate static library target name
- `LabV2`
  - the current workbench implementation living under `Studios/LabV2/`

## WASM Placement Rule

Current browser/wasm work should stay bounded:

- reusable browser build glue belongs in repo-level tooling such as `cmake/StudioSlabWasm.cmake`
- browser-safe target spikes belong in `Studios/WebGL-WASM/`
- future shared browser backend work belongs in shared platform layers, but only after desktop-only dependencies are partitioned cleanly

Do not treat `Studios/WebGL-WASM/` as the permanent architecture home for browser support.
It is the bounded proving ground.
