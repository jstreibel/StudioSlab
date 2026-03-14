# V2 Status Snapshot

## Snapshot Metadata

- Snapshot date: `2026-03-14`
- Last implementation update: `2026-03-14` (Model semantic graph + ODE realization `RZ-00..03`)
- Last architecture-doc update: `2026-03-14` (doc routing taxonomy + `RZ-03` handoff)
- Progress baseline: `Docs/v2-feature-backlog.md` progress notes dated `2026-03-14`
- Build-target sanity check date: `2026-03-14` (`StudioSlab` and `testsuite` build pass in `cmake-build-debug`)

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

## Recent Updates (`2026-03-06`, implementation)

- Graph substrate baseline landed in Reflection V2:
  - shared model under `Slab/Core/Reflection/V2/GraphSubstrateV2.h`
  - common vocabulary for graph mode/node/edge/port/member/policy/canvas metadata
  - graph document adapter projection from reflection catalogs
- LabV2 Graph Playground migration now uses shared substrate documents across all modes:
  - Semantic mode now builds/edits a substrate `FGraphDocumentV2` (spaces/operators + value-flow edges)
  - Template mode uses `FGraphDocumentV2` as source of truth (nodes, edges, canvas)
  - Runtime mode now renders/edit-pans a substrate `FGraphDocumentV2` built from merged reflection catalog snapshots
  - Routing mode uses `FGraphDocumentV2` edges with shared edge-kind enum and substrate-backed endpoint nodes
- LabV2 Schemes `Blueprint Graph` now consumes substrate-backed graph data:
  - per-interface graph document generated from reflection catalog
  - node view-models are built from substrate nodes
  - edge rendering is now driven by substrate edges instead of local ad-hoc link reconstruction
  - canvas pan/grid state is tracked on substrate canvas state
- Shared graph canvas renderer now backs Schemes and all Graph Playground modes:
  - extracted common canvas/pin/context/splitter behavior to `LabV2SubstrateGraphCanvas.cpp`
  - Template mode migrated off its legacy custom renderer path onto the shared substrate canvas path
  - Schemes `Blueprint Graph` migrated to the shared substrate canvas path while keeping policy badges, selection, and invoke/copy node actions
  - Schemes legacy right-click dummy creation menu for Plot window/artist was intentionally removed from graph canvas behavior
- Graph Playground first user-story slice advanced:
  - Template mode adds explicit `Instantiate Runtime` action producing a runtime instance graph artifact
  - Runtime mode supports dual view (`Reflection Snapshot` vs `Instantiated Graph`) with status + diagnostics + endpoint table
  - Routing mode consumes runtime instance endpoints, supports endpoint selection/import, and performs connect through an invocation path (`InvokeOperationV2`)
  - routing edges are added only on successful operation results; failures surface actionable diagnostics and operation telemetry
  - runtime instance session state (graph + diagnostics + view toggle) now persists to playground state file
- LabV2 window-manager decomposition started to control file growth:
  - Graph Playground logic moved behind `FLabV2GraphPlaygroundController` facade
  - Graph Playground draw and serialization moved into dedicated translation units
  - Schemes/plots panel draw paths moved to `LabV2WindowManagerSchemesPanels.cpp`

## Recent Updates (`2026-03-13`, implementation)

- Model V2 semantic-environment baseline landed:
  - `FModelV2` now carries model-level `BaseVocabulary` preset selection
  - ambient entries are structured semantic entries rather than display-only labels
  - current preset catalog:
    - `core_math`
    - `classical_mechanics`
    - `relativistic_field_theory`
- Resolution and diagnostics now distinguish:
  - local explicit definitions
  - active base-vocabulary entries
  - inferred / assumed local semantics
- Model workspace now exposes `Base Vocabulary` as a dedicated readonly semantic surface:
  - entries are inspectable and origin-aware
  - symbol-like entries render directly as LaTeX
  - notation-convention entries use preview LaTeX templates (for example `\\dot{\\mathrm{state}}`, `\\ddot{\\mathrm{state}}`, `\\partial_{\\mu}`)
- Seeded model alignment:
  - Harmonic Oscillator now uses `classical_mechanics`
  - Klein-Gordon now uses `relativistic_field_theory`
  - ambient `\\Box` is supplied by vocabulary instead of requiring only heuristic inference or a local `operator.box` definition
- Model workspace authoring/navigation pass landed:
  - semantic navigation/cross-highlighting now spans vocabulary, definitions, relations, assumptions, and inspector links
  - model-level semantic status/classification summary is computed from semantic overview data
  - draft semantic delta preview now compares canonical vs preview semantic state
  - draft-only assumptions/materializations are navigable before apply
  - direct creation flow exists for new local definitions and new local relations
  - Model workspace is split into dockable surfaces:
    - `Model Vocabulary`
    - `Model Definitions`
    - `Model Relations`
    - `Model Notation Editor`
    - `Model Assumptions`
    - `Model Inspector`
- Current authoring constraint:
  - unresolved-symbol diagnostics remain blocking errors for canonical apply
  - users resolve them by creating a local definition or materializing an inferred assumption

## Recent Updates (`2026-03-14`, implementation)

- Model semantic graph surface landed:
  - `LabV2` now opens a dedicated `Model Semantic Graph` plot window from the Model workspace summary surface
  - the graph is projected from `FModelSemanticOverviewV2`
  - graph selection feeds back into shared Model workspace selection
  - hover HUD and edge inspection are available
  - graph interaction currently includes pan/zoom, click selection, neighborhood hops, label toggle, and fit-to-graph
- Plot V2 interaction baseline extended:
  - artists can now participate in pointer hit-testing and keyboard event handling
  - the semantic-graph artist is the first built-in consumer of that interaction path
- ODE realization descent `RZ-00..02` is implemented:
  - one ODE realization descriptor is derived from canonical `Model V2` semantics
  - readiness is computed from semantic health, state coverage, and explicit model-level initial conditions
  - LabV2 `Model Layer` summary now exposes readiness, selected relations, diagnostics, and initial-state visibility
- ODE runtime bridge `RZ-03` is implemented:
  - one descriptor-driven bridge now maps explicit first-order ODE models into a runtime system plus `FSimulationRecipeV2` / `FStepperSessionV2`
  - the bridge stays narrow and requires an explicit numeric scalar-binding map for parameters and initial symbols
  - harmonic and damped oscillator runtime paths are covered by `ModelV2` tests
- Model realization validation now has the intended first boundary:
  - harmonic oscillator extracts cleanly
  - damped harmonic oscillator extracts cleanly
  - Klein-Gordon remains a blocked negative case for the ODE-first descent

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
- ODE realization descent status:
  - `RZ-00`: done
  - `RZ-01`: done
  - `RZ-02`: done
  - `RZ-03`: done

## Planned Next Slice (Follow-up)

- ODE-first descent from `Model V2` into `Realization` is now documented in:
  - `Docs/ode-realization-descent-plan.md`
- original `RZ-00`..`RZ-03` ladder is now implemented
- current recommendation:
  - add one minimal launch path from an ODE-ready model into the new runtime bridge
  - define the smallest authoring/config story for numeric scalar bindings required by the bridge
  - keep the first launch path to oscillator-family ODEs
  - keep PDE/field realization out of follow-up work

## Known Missing Pieces

- `Study` object model in code
- Sequence/presentation runtime in code
- LiveControl recording/replay and sequence-time integration
- Unified persistence/provenance model across assets/runs/studies
- Web/wasm target path (toolchain/build/backend split) is not implemented
- User-authored base-vocabulary editing and richer semantic-environment provenance are not implemented
- exact notation-span navigation from diagnostics/selection is not implemented
- dedicated initial-condition authoring/persistence flow is not implemented
- ODE-ready model launch path in `LabV2` is not implemented
- model-owned numeric scalar binding/parameter authoring for the runtime bridge is not implemented

## Conflict Rule

If a draft design doc disagrees with this file:
1. treat this snapshot as current implementation status
2. verify against code paths in `Slab/` and `Studios/LabV2/`
3. patch the stale doc
