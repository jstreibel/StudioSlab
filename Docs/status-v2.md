# V2 Status Snapshot

## Snapshot Metadata

- Snapshot date: `2026-03-02`
- Last implementation update: `2026-03-02` (listener-driven monitor snapshots + first XY/Ising live runtime parameter binding slice)
- Last architecture-doc update: `2026-03-02` (monitor data-path contract update: listener-pushed snapshots, `LatestOnly`)
- Progress baseline: `Docs/v2-feature-backlog.md` progress notes dated `2026-03-01`
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
  - first KG2D control-binding prototype paths (current LabV2 shell has no dedicated KG2D control-source panel)
- LabV2 shell:
  - launcher + run manager + data browser
  - view/panel management baseline
  - registry-driven top-level panel surfaces + extracted panel draw helpers (`LabV2Panels`)
  - passive monitor hosting for V2 slices

## Active Architecture Transition (Upcoming Work)

- StudioSlabV2 monitor/live-data path is moving to push-based snapshot delivery:
  - trigger -> listener copies snapshot -> monitor consumes latest (topic bridge optional)
- Monitor snapshot delivery target is `LatestOnly` (best-effort rendering; dropped intermediate frames are acceptable).
- New monitor-facing work should avoid direct session polling/read-lease loops in UI code.
- `FSessionLiveViewV2`/lease-oriented read paths remain compatibility mechanisms while slices migrate.

## Recent Updates (`2026-03-01`)

- Launcher UX polish:
  - removed `V2`-heavy naming from launcher labels/section headers
  - shortened per-example control labels (e.g. `L` instead of model-prefixed labels)
  - added tooltips for launcher action buttons
- Passive monitor windows:
  - V2 simulation monitor windows now default to plot-first composition (no built-in left telemetry pane)
  - plot controls remain available in-window (no forced `SetNoGUI()` path)
- Workspace routing hardening:
  - in docked mode, slab monitor windows render only in `Monitor` workspace
  - non-`Monitor` workspaces stop rendering/routing input to slab monitor windows
  - offscreen hide path keeps non-zero viewport dimensions to avoid repeated `glViewport w=0, h=0` warnings

## Recent Updates (`2026-03-02`)

- V2 monitor data path migration:
  - `FStateSnapshotListenerV2` now captures copied immutable snapshots behind a thread-safe envelope
  - snapshot subscriptions for monitor runs use `EDeliveryModeV2::LatestOnly`
  - passive monitor windows for SPI/KGRtoR/KGR2toR/MD/XY/Ising consume snapshot listeners instead of session lease polling in UI draw paths
- LabV2 launcher integration:
  - `SimulationManagerV2` monitor launches now attach snapshot subscriptions and pass snapshot listeners to monitor builders
- Live runtime parameter controls (first slice):
  - added generic scalar binding helpers (`LiveParameterControlV2`) for topic publish + runtime binding subscriptions
  - XY/Ising recipes expose atomic runtime control handles for hot-loop-safe reads (`Temperature`, `ExternalField`)
  - XY/Ising slice builders now publish read-only const parameter topics and bind mutable scalar controls from `LiveControl`
  - LabV2 launcher can optionally publish XY/Ising live parameter values while tasks run
- LabV2 shell cleanup and input routing hardening:
  - removed KG2D control-source panel surface and default monitor-side pre-publish path from LabV2 shell
  - mouse routing now honors ImGui capture for plot overlays (`Plot Detail` no longer propagates wheel/drag to the plot canvas behind it)

## Model Coverage Status (Current)

- `KG R^2->R`: `V2-H`, `V2-M`, first `V2-I` prototype path
- `SPI`: `V2-M`
- `Metropolis`: `V2-M`
- `Molecular Dynamics`: `V2-H`, `V2-M`
- `XY`: `V2-H`, `V2-M`, partial `V2-I` (live `Temperature`/`ExternalField`)
- `Ising`: `V2-H`, `V2-M`, partial `V2-I` (live `Temperature`/`ExternalField`)
- Functional minimization: first V2 slice currently active in planning/implementation track

## Active Workstream

- `CVG-04`: functional minimization V2 first slice (`Docs/functional-minimization-v2-slice-scope.md`)

## Known Missing Pieces

- `Study` object model in code
- Sequence/presentation runtime in code
- LiveControl recording/replay and sequence-time integration
- Unified persistence/provenance model across assets/runs/studies
- Web/wasm target path (toolchain/build/backend split) is not implemented

## Conflict Rule

If a draft design doc disagrees with this file:
1. treat this snapshot as current implementation status
2. verify against code paths in `Slab/` and `Studios/LabV2/`
3. patch the stale doc
