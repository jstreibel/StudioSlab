# V2 Status Snapshot

## Snapshot Metadata

- Snapshot date: `2026-03-01`
- Progress baseline: `Docs/v2-feature-backlog.md` progress notes dated `2026-02-27`
- Build-target sanity check date: `2026-03-01` (`StudioSlabV2`, `SlabTests`, `testsuite` present in `cmake-build-debug`)

## Implemented Baseline

- Numerics runtime V2 core:
  - `FSimulationRecipeV2`
  - `FSimulationSessionV2`
  - `FNumericTaskV2`
- Live data foundation:
  - `LiveData V2` topics/hub
  - `FSessionLiveViewV2` facade path
- Live control foundation:
  - `LiveControl V2` topics/hub
  - first KG2D control-binding prototype paths
- LabV2 shell:
  - launcher + run manager + data browser
  - view/panel management baseline
  - passive monitor hosting for V2 slices

## Model Coverage Status (Current)

- `KG R^2->R`: `V2-H`, `V2-M`, first `V2-I` prototype path
- `SPI`: `V2-M`
- `Metropolis`: `V2-M`
- `Molecular Dynamics`: `V2-H`, `V2-M`
- `XY`: `V2-H`, `V2-M`
- `Ising`: `V2-H`, `V2-M`
- Functional minimization: first V2 slice currently active in planning/implementation track

## Active Workstream

- `CVG-04`: functional minimization V2 first slice (`Docs/functional-minimization-v2-slice-scope.md`)

## Known Missing Pieces

- `Study` object model in code
- Sequence/presentation runtime in code
- LiveControl recording/replay and sequence-time integration
- Unified persistence/provenance model across assets/runs/studies

## Conflict Rule

If a draft design doc disagrees with this file:
1. treat this snapshot as current implementation status
2. verify against code paths in `Slab/` and `Studios/LabV2/`
3. patch the stale doc
