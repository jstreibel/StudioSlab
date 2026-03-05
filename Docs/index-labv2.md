# Index: LabV2 Shell and UI Surfaces

## Use This When

- Working on `StudioSlabV2` startup or shell behavior
- Adding/changing LabV2 panels, menus, or layout/orchestration
- Wiring LabV2 launch actions to V2 simulation slices
- Debugging LabV2 event routing or plot-control UX behavior

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `SPEC.md` (LabV2 role and V2 boundaries)
3. UI/runtime contracts:
   - `Docs/graphics-composition-contract.md`
   - `Docs/plot-window-ux-contract.md`
4. Active planning context:
   - `Docs/v2-feature-backlog.md`

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
- Plot reflection catalog bridge surfaces:
  - `Slab/Core/Reflection/V2/ReflectionCatalogRegistryV2.h`
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

## Current UI Invariants (Docked Mode)

- Slab monitor windows are rendered/interactive only in the `Monitor` workspace.
- `Simulations` and `Schemes` workspaces should not show monitor slab-window leftovers.
- Simulation launcher labels should avoid `V2` prefixing and model-specific clutter where a generic control label is sufficient.

## Validation Minimum

- Build:
  - `cmake --build cmake-build-debug --target StudioSlabV2 SlabTests -j8`
- Runtime smoke:
  - `./Build/bin/StudioSlabV2`
  - `./Build/bin/SlabTests list`
- If changing ImGui/layout behavior:
  - `Scripts/imgui-runtime-smoke.sh`
  - `Scripts/imgui-provider-runtime-matrix.sh`
