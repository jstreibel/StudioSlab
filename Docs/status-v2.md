# V2 Status Snapshot

## Snapshot Metadata

- Snapshot date: `2026-04-05`
- Last implementation update: `2026-04-05` (`Studios/WebApp` GitHub Pages deployment workflow)
- Last architecture-doc update: `2026-04-03` (windowing migration plan/status refresh)
- Progress baseline: `Docs/v2-feature-backlog.md` progress notes dated `2026-03-14`
- Build-target sanity check date: `2026-04-03` (`StudioSlab` builds in `cmake-build-debug` after the shared workspace shell extraction; `emcmake` configure plus `WebGLWasmSandbox` / `WasmImGuiSandbox` / `WasmWorkspaceSandbox` / `WasmIsingWorkspaceSandbox` build locally in `cmake-build-webgl-wasm` with `Emscripten 3.1.6`; `[Plot2DV2]` still fails on an existing HUD-text assertion)

## Recent Updates (`2026-04-05`, web app deployment)

- Added `.github/workflows/deploy-webapp-pages.yml` to publish `Studios/WebApp` to GitHub Pages on pushes to `main`.
- The workflow builds the bounded wasm dependency first:
  - initializes the `Lib/3rdParty/imgui` submodule only
  - installs Emscripten `3.1.6`
  - builds `WasmIsingWorkspaceSandbox`
- After the wasm artifacts exist, the workflow builds `Studios/WebApp`, uploads `Studios/WebApp/dist`, and deploys it through the GitHub Pages Actions path.
- The intended default published URLs are:
  - `https://jstreibel.github.io/StudioSlab/`
  - `https://jstreibel.github.io/StudioSlab/wasm/ising-workspace-wasm-sandbox.html`

## Recent Updates (`2026-04-05`, wasm page title)

- `WasmIsingWorkspaceSandbox` now uses a target-local Emscripten shell template at `Studios/WebGL-WASM/ising_shell.html`.
- The browser page title for the Ising wasm target is now `StudioSlab Ising study` instead of the default Dear ImGui Emscripten example title.
- The Ising wasm sandbox now caps lattice size at `L = 127`, which is the highest safe value for the current single-draw-list spin-lattice renderer.

## Recent Updates (`2026-04-04`, web app scaffold)

- Added `Studios/WebApp/` as a local React frontend workspace for StudioSlab:
  - Vite 6 + React 19 + TypeScript
  - Redux Toolkit state spine
  - Tailwind CSS + local app CSS
- The minimal landing page now includes launch cards for:
  - `Ising Model`, which opens the existing wasm sandbox from the local web shell
  - `GitHub`, which links out to the public repository page
- `Studios/WebApp` now bridges the built `WasmIsingWorkspaceSandbox` artifacts into Vite dev/build output without introducing a root JS workspace.
- The JS toolchain remains app-local:
  - no root `package.json`
  - no root JS workspace manager
  - no CMake coupling
- This is a generic StudioSlab web shell, not a `LabV2` browser port and not a replacement for the bounded wasm sandboxes.

## Recent Validation (`2026-04-03`, wasm)

- Verified `emcmake cmake -S Studios/WebGL-WASM -B cmake-build-webgl-wasm` succeeds locally.
- Verified all bounded browser targets build locally:
  - `WebGLWasmSandbox`
  - `WasmImGuiSandbox`
  - `WasmWorkspaceSandbox`
  - `WasmIsingWorkspaceSandbox`
- Expected `.html`, `.js`, and `.wasm` artifacts are emitted under `cmake-build-webgl-wasm/Build/bin/`.
- Prior `emrun --no_browser` serving validation remains in place for the first two sandboxes; concurrent sandbox launches still need distinct `--port` values to avoid default-port collisions.

## Recent Updates (`2026-04-04`, wasm Ising workspace layout)

- `WasmIsingWorkspaceSandbox` now routes `Observable History` into the `Simulation` workspace instead of the `Observables` workspace.
- The default `Simulation` dock stack now groups:
  - `Run Inspector`
  - `Observable History`
  - `Event Console`
- `Observable History` is the default selected tab in that stack and the stack height was increased to give the history plots more room.
- A shared `History points` control now lives in `Browser-local Metropolis kernel` and caps both `Observable History` plots and the `External field x magnetization` trace with the same retained sample count.
- `Browser-local Metropolis kernel` now keeps `L`, `T`, `h`, and `Seed` in the top block, moves the remaining run/history controls below the first separator, and mirrors the preset actions below a second separator.
- The default `Simulation` top row now docks `Hysteresis Trace` immediately to the right of `Spin Lattice` with an equal-width split, while `Phase Snapshot` remains on the far-right rail and the bottom history stack stays beneath the center trace area.
- The `Observables` workspace now keeps the lighter two-panel arrangement:
  - `Sampling Controls`
  - `Sweep Statistics`

## Recent Updates (`2026-03-29`, platform/tooling)

- Added reusable Emscripten browser-target glue at `cmake/StudioSlabWasm.cmake`.
- `Studios/WebGL-WASM/` now carries two bounded browser targets:
  - `WebGLWasmSandbox`
  - `WasmImGuiSandbox`
- `FPlot2DWindowHostV2` no longer hardcodes one concrete render backend instance and now accepts injected `IPlotRenderBackendV2` implementations while defaulting to the current OpenGL backend.
- This is still not a `LabV2` browser port; desktop graphics/dependency partitioning remains the main blocker.

## Recent Updates (`2026-04-02`, shared windowing extraction)

- Default editor-style windowing migration plan added:
  - `Docs/labv2-default-windowing-migration-plan.md`
- First shared shell/layout slice landed under `Slab`:
  - `Slab/Graphics/Window/V2/WorkspaceLayoutV2.*`
  - shared workspace definitions + dock-layout recipe vocabulary
  - shared ImGui dock-layout application helper
- `LabV2` now consumes the shared workspace/dock recipe layer for its default docked layout instead of hardcoding all dock splits directly inside one window-manager method.
- `FHostedSurfaceV2` is now the public hosted-surface direction for editor-style windowing.
- desktop plot/editor surfaces still render through the existing `FSlabWindow` / `FPlot2DWindowHostV2` path, now wrapped by the hosted-surface contract instead of replacing it.

## Recent Updates (`2026-04-03`, shared workspace shell extraction)

- `LAB-18` advanced through the WN-01 shell layer:
  - added `Slab/Graphics/Window/V2/WorkspaceShellV2.*`
  - shared workspace launcher, tab strip, workspace visibility strip, dockspace host, and panel-surface draw helpers now live under `Slab`
  - shared per-workspace visibility persistence moved out of `FLabV2WindowManager`
- `LabV2` now consumes the shared shell contract by supplying:
  - workspace definitions
  - workspace visibility items
  - panel registrations
  - the simulation-launcher initial-dock hook
- slab-window routing still remains partially local to `LabV2`; the next boundary is widening hosted-surface coverage beyond plot hosts and then converging more of the remaining raw `FSlabWindow` glue.

## Recent Updates (`2026-04-03`, hosted surface extraction)

- `LAB-19` started with the public hosted-surface direction now locked to `FHostedSurfaceV2`:
  - added `Slab/Graphics/Window/V2/HostedSurfaceV2.*`
  - added `FSlabHostedSurfaceV2` as the shared adapter over the current `FSlabWindow` lifecycle
- `LabV2` plot-host routing now uses hosted surfaces for:
  - plot-host discovery/sync
  - per-surface visibility checks
  - focus preparation before bringing hosted slab windows forward
- this is still an extraction, not a pipeline rewrite:
  - hosted plot surfaces continue rendering through `FPlot2DWindowHostV2`
  - generic non-plot slab windows still remain on the raw `FSlabWindow` path

## Recent Updates (`2026-04-03`, wasm workspace sample)

- Added `WasmWorkspaceSandbox` under `Studios/WebGL-WASM/`:
  - browser-hosted sample that reuses `WorkspaceLayoutV2` + `WorkspaceShellV2`
  - demonstrates top launcher, top tabs, workspace visibility strip, docked panels, and browser-safe preview surfaces
- The shared windowing shell now has one bounded wasm proof point without depending on the desktop `FSlabWindow` stack.
- This is still a shell sample, not a browser plot renderer:
  - preview surfaces are immediate-mode ImGui draw-list placeholders
  - `IPlotRenderBackendV2` / WebGL2 plot-backend migration remains future work

## Recent Updates (`2026-04-03`, wasm Ising workspace sample)

- Added `WasmIsingWorkspaceSandbox` under `Studios/WebGL-WASM/`:
  - reuses the shared V2 workspace shell and theme launcher
  - hosts a browser-local 2D Ising Metropolis workspace with docked controls, lattice view, observables, presets, and phase snapshot panels
- This remains a bounded browser sample:
  - Ising stepping is local to the wasm target instead of reusing the full desktop V2 runtime/session stack
  - desktop live-data monitors and browser Plot2D/WebGL2 backend migration are still future work

## Recent Updates (`2026-03-22`, implementation)

- Ontology workspace navigation was reworked from one overloaded graph into two coordinated plot surfaces:
  - `Ontology Overview`
  - `Ontology Focus`
  - plus the existing `Ontology Layer` and `Ontology Inspector`
- The overview graph now renders as a compact ontology map:
  - one-line node labels
  - no category/footer text in-node
  - quieter unlabeled edges by default
  - selection highlight without forcing the full graph into inspector-card mode
- The focus graph now derives a neighborhood projection from the shared selection:
  - defaults to the selected study root when nothing is explicitly selected
  - shows a first-hop neighborhood by default
  - uses the richer readable card treatment and edge labels
- Projection correctness fix:
  - non-selected study-local nodes are no longer admitted into the selected-study projection with default `(0, 0)` positions
  - this removes a major hidden source of ontology node pile-ups/overlap in the previous screenshots

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
  - `LabV2` now opens a dedicated `Model Semantic Graph` `Plot V2` surface from the Model workspace summary surface
  - that graph surface now lives in the `Models` workspace, is visible by default, and docks directly below `Model Notation Editor`
  - the graph is projected from `FModelSemanticOverviewV2`
  - graph selection feeds back into shared Model workspace selection
  - hover HUD and edge inspection are available
  - graph interaction currently includes pan/zoom, click selection, neighborhood hops, label toggle, and fit-to-graph
  - the `Plots` workspace no longer owns this semantic-graph surface
  - Model workspace can now export the selected-object-centered semantic graph neighborhood to JSON in the process current working directory (`pwd`) for offline inspection/sharing
  - export payload includes plotted nodes/edges plus per-node semantic metadata, diagnostics, and navigation-link groups
- Plot V2 interaction baseline extended:
  - artists can now participate in pointer hit-testing and keyboard event handling
  - the semantic-graph artist is the first built-in consumer of that interaction path
- Plot V2 host/control parity first slice landed:
  - LabV2 V2-plot hosts now expose legacy-style floating toolbar / edge-strip controls plus the attached detail panel instead of acting as render-only wrappers
  - V2 artist z-order is now surfaced through reflection and editable from host-side layer controls
  - `query.window.list_artists` for V2 windows now reports per-artist id/label/z-order/visibility metadata
- Plot V2 host/artist HUD harmony pass landed:
  - `FPlotFrameContextV2` now carries a host-published `FPlotHudLayoutV2` contract for screen-space artist overlays
  - `FPlot2DWindowHostV2` publishes anchors that account for the floating toolbar, right-edge strip, and attached detail panel
  - `FModelSemanticGraphArtistV2` now anchors selection/hover HUD cards to the bottom-left zone and clamps the status card to the top-right safe edge instead of hardcoding the viewport corners
  - `FPlotFrameContextV2` now also carries writer-derived text metrics so artist HUD cards can size/pad themselves from the active font instead of fixed pixel literals
- Plot V2 shared-host extraction landed:
  - `FPlot2DWindowHostV2` now lives under `Slab/Graphics/Plot2D/V2/` instead of being embedded in `LabV2WindowManager.cpp`
  - duplicated dead host copies were removed from the LabV2 Graph Playground / Schemes translation units
  - `StudioSlab` and `testsuite` still build after the extraction, and `[Plot2DV2]` remains green
- Plot V2 `R2Section` parity slice landed:
  - `FR2SectionArtistV2` now ports the legacy sampled section-curve role into the backend-neutral V2 draw-list path
  - per-section legend entries plus per-section visibility/style controls are exposed through the generic reflection overlay path
  - `[Plot2DV2]` coverage now includes V2 `R2Section` draw output, bounds, and reflection mutation checks
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

## Recent Updates (`2026-03-15`, implementation)

- Model semantic graph layering baseline landed in code:
  - shared `Model V2` semantic graph projection now distinguishes canonical versus overlay nodes/edges
  - Plot V2 semantic graph artist now renders that shared projection instead of rebuilding graph semantics locally
  - Model semantic graph JSON export now serializes the same layered projection, including per-node/per-edge layer metadata
- Layering contract is now aligned across implementation docs:
  - `Docs/index-model-semantic-graph.md`
  - `Docs/model-semantic-graph-layering.md`
  - `Docs/model-v2-semantic-environment.md`

## Recent Updates (`2026-03-16`, implementation)

- Harmonic-oscillator energy-role mismatch was narrowed to the intended ontology boundary:
  - resolved declared `Coordinate`, `StateVariable`, `Field`, and `ObservableSymbol` meanings are now preserved when local algebraic syntax alone would have flattened them to scalar-like roles
  - this clears the false `obs.energy` mismatch in the harmonic-oscillator semantic reports used by the Model graph/JSON path
- Semantic-level guidance is now documented in a minimal indexed form:
  - `Docs/index-model-semantic-graph.md`
  - `Docs/model-semantic-graph-layering.md`
  - `Docs/model-semantic-levels.md`

## Recent Updates (`2026-03-16`, implementation, ODE follow-up)

- Model workspace ODE runtime follow-up landed:
  - `Model Layer` now exposes a minimal lab-local numeric-binding surface for the scalar symbols required by the descriptor-driven ODE runtime bridge
  - required binding order is now shared/tested through `CollectODEExplicitFirstOrderRequiredScalarBindingsV2(...)`
  - binding defaults are seeded only for the current oscillator-family launch slice; values remain outside canonical `Model V2` semantics
- First LabV2 launch path from `Model` into the runtime bridge landed:
  - `SimulationManagerV2` now accepts an already-built `FSimulationRecipeV2` for last-mile task launch
  - `Model Layer` can launch oscillator-family ODE-ready models through the existing `RZ-03` bridge as headless numeric tasks
  - the new path is intentionally narrow: no dedicated monitor/live-data visualization path yet, and no PDE/field broadening

## Recent Updates (`2026-03-18`, implementation, ontology graph viewer)

- Split-file ontology graph inspection landed in StudioSlab:
  - ontology resources are loaded from `Resources/Ontologies`
  - supported document set:
    - `studioslab-ontology.schema.json`
    - `global-descent.ontology.json`
    - one or more `*.study.json`
  - loader/projection path lives in `Slab/Core/Ontology/V2/OntologyGraphV2.h`
  - read-only Plot V2 rendering lives in `Slab/Graphics/Plot2D/V2/Artists/OntologyGraphArtistV2.*`
- LabV2 now exposes a dedicated `Ontology` workspace:
  - `Ontology Graph`
  - `Ontology Layer`
  - `Ontology Inspector`
  - the ontology surface no longer rides inside the `Model` workspace
- First ontology graph slice includes:
  - bundle construction over schema/global/study documents
  - soft validation and diagnostics
  - deterministic layered layout
  - selected-study projection over the persistent global ontology
  - filters for scope, active region, blocked requirements, and realization/recipe/artifact path
  - click/hover selection with provenance/raw-payload inspector data
- Coverage and examples:
  - harmonic oscillator readable ODE -> RK4 -> artifact path
  - Klein-Gordon blocked requirements and reachable PDE realization path
  - dedicated tests added in `Lib/tests/test_ontology_graph_v2.cpp`
- Ontology graph text sizing/zoom follow-up landed:
  - Plot V2 now honors `FTextCommandV2::FontScale` in the OpenGL backend
  - ontology plot-space labels and badges scale with graph zoom
  - ontology node measurement uses the same zoom-derived scale so boxes and labels stay coherent
  - current implementation uses transformed glyph geometry; SDF/freetype-gl extension remains a later quality path if wider zoom ranges require it

## Recent Updates (`2026-03-20`, implementation, ontology graph layout polish)

- Ontology workspace graph readability pass landed:
  - deterministic layout now reserves materially wider columns and study sidecars for long ontology cards and badge rows
  - dense titles are wrapped into compact two-line cards instead of stretching the graph horizontally
  - ontology edge labels now default to off for the dense overview path and remain available through the existing toggle/hover flow
- LabV2 ontology framing is now projection-aware:
  - when studies or filters rebuild the ontology projection, the Plot V2 host requests a fit-to-graph pass instead of leaving the window on a stale zoom region
  - the ontology workspace dock split now gives the graph more horizontal and vertical room by default
- Coverage follow-up:
  - `Lib/tests/test_ontology_graph_v2.cpp` now also guards the denser column-spacing baseline used by the ontology layout pass

## Recent Updates (`2026-03-21`, implementation, ontology graph compact overview)

- Ontology graph overview density was reduced further:
  - fitted/overview zoom now renders ontology cards at a compact text scale instead of treating the overview like an inspector view
  - node cards collapse to title-first summaries in the dense overview and reveal footer metadata on hover, selection, highlight, or zoom-in
  - minimum ontology card geometry was reduced so the overview reads as a graph again instead of a wall of equal-sized panels
- The ontology surface keeps the previous deterministic layout and fit-on-projection-change path:
  - this pass is intentionally a render-density pass, not a new force/layout algorithm
  - deterministic layer columns were then compacted horizontally so the overview stops burning width on conservative lane spacing that no longer matches the smaller cards
  - study-local sidecars are now reserved only on layers the selected study actually occupies, eliminating empty placeholder lanes in the fitted overview
  - overview node geometry now follows the same sub-`1.0` text scale as the rendered labels, so cards stop keeping full-size padding after the text has shrunk
  - ontology cards now render friendly public categories such as `semantic`, `study`, `requirement`, and `solver` instead of raw schema kinds like `SemanticClass` or `SolverClass`
  - category meaning is pushed further into presentation: requirement cards use a chamfered silhouette, study cards use a tabbed silhouette, and category colors distinguish requirement vs descent and solver vs recipe
  - targeted ontology coverage still passes after the overview/detail split

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
  - keep the current model-driven launch path narrow and stable
  - decide whether numeric bindings should remain lab-local or gain a model-owned authoring story
  - add a dedicated monitor/live-data presentation only when there is a concrete ODE-runtime visualization target
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
- model-owned numeric scalar binding/parameter authoring for the runtime bridge is not implemented
- dedicated monitor/live-data visualization for model-driven ODE runtime runs is not implemented
- ontology graph editing / JSON write-back is not implemented
- multi-study ontology comparison is not implemented

## Conflict Rule

If a draft design doc disagrees with this file:
1. treat this snapshot as current implementation status
2. verify against code paths in `Slab/` and `Studios/LabV2/`
3. patch the stale doc
