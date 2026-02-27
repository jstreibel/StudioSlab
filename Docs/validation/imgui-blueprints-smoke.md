# ImGui + Blueprints Smoke Validation

## Purpose
- Validate that the Blueprint UI keeps working while layout compatibility waves are in flight.
- Keep checks short and repeatable after each migration commit.

## Build
- Configure and build with your target profile.
- Required green targets:
  - `cmake --build <build-dir> --target Slab`
  - `cmake --build <build-dir> --target Studios`

## Runtime Checks

### 1) Blueprint Prototype App
- Run: `./Build/bin/Blueprints-prototype`
- Validate:
  - Node panels render without overlap corruption.
  - Nodes can be selected and moved.
  - Links can be created and removed.
  - Context menu actions still open.

### 2) Lab App (legacy)
- Run: `./Build/bin/StudioSlab`
- Validate:
  - Blueprint screen still opens.
  - Left pane and graph/editor panes split and resize correctly.
  - No assertion or crash when interacting with links and nodes.

### 3) LabV2 App (non-blueprint guard)
- Run: `./Build/bin/StudioSlabV2`
- Validate:
  - Main UI renders and remains interactive.
  - No global ImGui regressions (menus, panels, plots).

## Automated Runtime Smoke
- Provider matrix (upstream default + legacy-flag build):
  - `Scripts/imgui-provider-runtime-matrix.sh`
- Upstream spike runtime smoke:
  - `Scripts/imgui-upstream-runtime-spike.sh v1.91.0`
- Test-only smoke on current binary:
  - `Scripts/imgui-runtime-smoke.sh`

## Provider Matrix
- Default mode: `-DSTUDIOSLAB_IMGUI_LAYOUT_USE_FORK=OFF`
- Legacy-flag mode: `-DSTUDIOSLAB_IMGUI_LAYOUT_USE_FORK=ON` (deprecated)
- Minimum requirement for each wave:
  - both modes compile,
  - default mode passes full runtime checks,
  - legacy-flag mode passes at least launch + basic interaction.
