# Index: LabV2 Shell and UI Surfaces

## Use This When

- Working on `StudioSlab` startup or `LabV2` shell behavior
- Adding/changing LabV2 panels, menus, or layout/orchestration
- Wiring LabV2 launch actions to V2 simulation slices
- Debugging LabV2 event routing or plot-control UX behavior

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `SPEC.md` (LabV2 role and V2 boundaries)
3. UI/runtime contracts:
   - `Docs/graphics-composition-contract.md`
   - `Docs/plot-window-ux-contract.md`
   - `Docs/typesetting-bridge.md`
4. For Model-tab semantic-environment work:
   - `Docs/model-v2-semantic-environment.md`
5. Active planning context:
   - `Docs/v2-feature-backlog.md`
   - `Docs/graph-first-user-story-checklist.md`
6. For Schemes/graph-surface semantics and mode boundaries:
   - `Docs/graph-onthology.md`

## Core Code Map

- Target wiring:
  - `Studios/LabV2/CMakeLists.txt`
  - `Studios/LabV2/MainStudioSlabV2.cpp`
- App bootstrap:
  - `Studios/LabV2/StudioSlabV2App.h`
  - `Studios/LabV2/StudioSlabV2App.cpp`
- Top-level shell/window orchestration:
  - `Studios/LabV2/LabV2WindowManager.h`
  - `Studios/LabV2/LabV2WindowManager.cpp`
  - `Studios/LabV2/LabV2WindowManagerModelPanels.cpp`
  - `Studios/LabV2/LabV2WindowManagerSchemesPanels.cpp`
  - `Studios/LabV2/LabV2WindowManagerGraphPlayground.cpp`
  - `Studios/LabV2/LabV2WindowManagerGraphPlaygroundSerialization.cpp`
  - `Studios/LabV2/LabV2SubstrateGraphCanvas.cpp`
  - `Studios/LabV2/LabV2GraphPlaygroundController.h`
  - `Studios/LabV2/LabV2GraphPlaygroundController.cpp`
- Plot V2 semantic-graph surface:
  - `Slab/Graphics/Plot2D/V2/PlotArtistV2.h`
  - `Slab/Graphics/Plot2D/V2/Plot2DWindowV2.h`
  - `Slab/Graphics/Plot2D/V2/Artists/ModelSemanticGraphArtistV2.h`
- Plot reflection catalog bridge surfaces:
  - `Slab/Core/Reflection/V2/ReflectionCatalogRegistryV2.h`
  - `Slab/Core/Reflection/V2/GraphSubstrateV2.h`
  - `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.h`
  - `Slab/Graphics/Plot2D/V2/LegacyPlotReflectionAdapterV2.h`
- Shared LabV2 panel draw surfaces:
  - `Studios/LabV2/LabV2Panels.h`
  - `Studios/LabV2/LabV2Panels.cpp`
- Simulation launch surface:
  - `Studios/LabV2/SimulationManagerV2.h`
  - `Studios/LabV2/SimulationManagerV2.cpp`
- Shared LabV2-adjacent helpers:
  - `Slab/Studios/Common/VisualHost.h`
  - `Slab/Studios/Common/SessionLiveViewStatsV2.h`
- Typesetting bridge:
  - `Docs/typesetting-bridge.md`
  - `Lib/Graphics/Typesetting/TypesettingService.h`
  - `Lib/Graphics/Typesetting/ImGuiTypesetting.h`
  - `Studios/LabV2/LabV2WindowManagerModelPanels.cpp`
- Model semantic environment:
  - `Docs/model-v2-semantic-environment.md`
  - `Slab/Core/Model/V2/ModelTypesV2.h`
  - `Slab/Core/Model/V2/ModelNotationV2.h`
  - `Slab/Core/Model/V2/ModelAuthoringV2.h`
  - `Slab/Core/Model/V2/ModelRealizationV2.h`
  - `Slab/Core/Model/V2/ModelSeedsV2.h`
  - `Lib/tests/test_model_v2.cpp`

## Change Patterns

- Add new simulation launcher action:
  1. update `SimulationManagerV2.*`
  2. reuse/create `Slab/Studios/Common/Simulations/V2/*`
  3. reuse/create `Slab/Studios/Common/Monitors/V2/*` if monitored path is needed
- Add/change top-level panel behavior:
  1. update `LabV2WindowManager.*`
  2. keep top-level panel ownership and docking in `LabV2WindowManager.*`
  3. keep reusable panel draw logic in `LabV2Panels.*`
  4. keep numerics logic inside shared simulation/runtime modules
- Add/change Model workspace behavior:
  1. update `LabV2WindowManagerModelPanels.cpp`
  2. keep dock titles/visibility/default layout in `LabV2WindowManager.cpp`
  3. keep semantic preview/create/apply logic in `ModelAuthoringV2.h`
  4. keep coverage in `Lib/tests/test_model_v2.cpp`

## Current UI Invariants (Docked Mode)

- Slab monitor windows are rendered/interactive only in the `Monitor` workspace.
- `Simulations` and `Schemes` workspaces should not show monitor slab-window leftovers.
- Simulation launcher labels should avoid `V2` prefixing and model-specific clutter where a generic control label is sufficient.
- In the Model workspace, semantic concerns are split into dockable windows instead of one monolithic panel.
- The Model workspace visibility toggle labeled `Summary` maps to the panel titled `Model Layer`.
- `Model Layer` owns the high-level semantic summary, the `Semantic Graph -> Open in Plots` entry point, and the ODE realization readiness summary.
- The Model window order should remain conceptually stable:
  - `Vocabulary`
  - `Definitions`
  - `Relations`
  - `Notation Editor`
  - `Assumptions`
  - `Inspector`
- In the Model workspace, `Base Vocabulary` is ambient/readonly and full object details belong in `Model Inspector`, not repeated in catalog panes.
- The `Model Semantic Graph` plot window is a read-only navigation surface tied to shared model selection.
- New model content creation is transactional:
  - preview first
  - canonical mutation only on `Create` / `Apply`
  - unresolved-symbol errors block apply until resolved or materialized

## Validation Minimum

- Build:
  - `cmake --build cmake-build-debug --target StudioSlab testsuite -j8`
- Runtime smoke:
  - `./Build/bin/StudioSlab`
  - `./Build/bin/testsuite "[ModelV2]"`
  - `./Build/bin/testsuite "[Plot2DV2]"`
- If changing ImGui/layout behavior:
  - `Scripts/imgui-runtime-smoke.sh`
  - `Scripts/imgui-provider-runtime-matrix.sh`
