# V2 Status Snapshot

## Snapshot Metadata

- Snapshot date: `2026-03-05`
- Last implementation update: `2026-03-05` (Plot2D legacy-to-V2 reflection bridge + legacy discovery/id hooks)
- Last architecture-doc update: `2026-03-05` (catalog registry docs + indexed technical debt follow-ups)
- Progress baseline: `Docs/v2-feature-backlog.md` progress notes dated `2026-03-04`
- Build-target sanity check date: `2026-03-04` (`StudioSlab`, `Studios`, `SlabTests`, `testsuite` present in `cmake-build-debug`)

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

- StudioSlab/LabV2 monitor/live-data path is moving to push-based snapshot delivery:
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
  - `SessionLiveViewV2` now exposes a snapshot topic (`TryGetSnapshot`) for monitor-facing render payloads
  - snapshot publication is consumer-gated (monitor open/close controls copy pressure)
  - passive monitor windows for SPI/KGRtoR/KGR2toR/MD/XY/Ising/Metropolis consume snapshots instead of session lease polling in UI draw paths
- LabV2 launcher integration:
  - `SimulationManagerV2` monitor launches build `SessionLiveViewV2` topics that feed passive monitor snapshots
- Live runtime parameter controls (first slice):
  - added generic scalar binding helpers (`LiveParameterControlV2`) for topic publish + runtime binding subscriptions
  - XY/Ising recipes expose atomic runtime control handles for hot-loop-safe reads (`Temperature`, `ExternalField`)
  - XY/Ising slice builders now publish read-only const parameter topics and bind mutable scalar controls from `LiveControl`
  - LabV2 launcher can optionally publish XY/Ising live parameter values while tasks run
- LabV2 shell cleanup and input routing hardening:
  - removed KG2D control-source panel surface and default monitor-side pre-publish path from LabV2 shell
  - mouse routing now honors ImGui capture for plot overlays (`Plot Detail` no longer propagates wheel/drag to the plot canvas behind it)

## Recent Updates (`2026-03-04`, design lock)

- Reflection V2 design baseline locked (docs/contracts):
  - added indexed route for reflection workstream (`Docs/index-reflection-v2.md`)
  - added contract baseline (`Docs/reflection-v2-contract.md`)
  - added adapter-first migration plan (`Docs/reflection-v2-migration-plan.md`)
- Operation semantics decision:
  - unified operation model (`Kind = Command | Query`) with one invocation path
  - no heavy CQRS split introduced
- Parameter policy baseline extended:
  - `RestartRequired` mutability class formalized for staged apply-on-restart flows

## Recent Updates (`2026-03-04`, implementation)

- Reflection V2 phases `RV2-01`..`RV2-04` vertical slice implemented:
  - shared reflection runtime/adapter/invoke path under `Slab/Core/Reflection/V2/`
  - LabV2 Schemes now exposes two surfaces in `Studios/LabV2/LabV2WindowManager.cpp`:
    - `Interface Inspector` (reflection catalog + invocation/editor panel)
    - `Blueprint Graph` (node-canvas graph for selected interface)
  - new CLI reflection command path in `Studios/CLI/main.cpp` (`Studios reflect ...`)
- Operation/runtime contract path now exercised in both consumers:
  - command/query operations: parameter list/get/set/apply
  - invoke-time checks: `ThreadAffinity`, `RunStatePolicy`, query side-effect invariant
- First complex parameter slice implemented:
  - `slab.math.function.r_to_r` descriptor codec + editor/apply flow via sandbox interface
  - `RestartRequired` staged apply behavior wired through shared adapter operations
- Schemes Blueprint Graph UX iteration:
  - graph now splits operation lanes into `Queries` and `Commands` (with interface-centered flow)
  - in-graph policy/state badges added (parameter mutability/exposure/live-draft-pending and operation runtime/thread/side-effect hints)
  - node-level quick actions added (`Get`/`Set`/`Apply`/`Invoke` + copy id) through shared reflection invoke path
  - graph-side operation trace rail added for recent invoke outcomes and latency visibility
  - graph controls added for search and visibility filters (`Parameters`/`Queries`/`Commands`, mutability filter)

## Recent Updates (`2026-03-05`, implementation)

- Plot2D reflection catalog now composes both sources:
  - V2-native windows/artists (`v2.plot.*`)
  - legacy Plot2D windows/artists via adapter bridge (`legacy.plot.*`)
- Reflection catalog registry baseline implemented:
  - central source registry in `Slab/Core/Reflection/V2/ReflectionCatalogRegistryV2.h`
  - LabV2 Schemes now consumes merged catalogs across all registered sources (not legacy-only)
  - Schemes invoke/live-value reads are routed by interface source through registry
- Legacy Plot2D hooks added for safe adapter projection:
  - window discovery (`GetLiveWindows`)
  - stable ids (`GetStableWindowIdV2`, artist ids)
  - artist slot/z-order metadata access
  - artist parameter get/set extension points for bridge-driven reflection
- Plot inspector empty-state message now reflects mixed-source discovery (V2 + legacy).

## Reflection Working-Memory Caveats (Current)

- Schemes `Blueprint Graph` is a visualization/navigation surface, not yet a full graph-execution editor.
- Graph node layout state is not persisted across restarts.
- Reflection ids intentionally mix legacy snake_case compatibility and V2 kebab-case style.
- Reflection catalog availability depends on in-process legacy interface registration timing.

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
- Reflection V2 migration workstream status:
  - `RV2-00`: done
  - `RV2-01`: done
  - `RV2-02`: done
  - `RV2-03`: done
  - `RV2-04`: done

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
