# LabV2 Default Windowing Migration Plan

## Goal

Turn the current `StudioSlab` / `LabV2` shell into the default shared windowing pattern for editor-style studios:

- top workspace tabs
- docked tool windows around the edges
- hosted slab surfaces in the center
- `Plot2D V2` as the first canonical hosted surface

This plan also defines the renderer migration needed for browser-safe plot rendering through WebGL2 / wasm.

## Design Decisions Locked

1. Shared shell/layout code moves into `Slab/Graphics/Window/V2/`.
- layout metadata, workspace definitions, and dock recipes are shared platform code
- studio-specific panel content stays under `Studios/LabV2/`

2. `FPlot2DWindowV2` remains the scene/data model.
- it keeps window region, viewport, artists, hit-testing, and draw-list production
- it does not absorb ImGui dock/layout concerns

3. `FPlot2DWindowHostV2` remains the first concrete hosted-surface path.
- near-term migration should build on the existing host instead of inventing a second plot host
- any hosted-surface adapter must wrap the current host/event lifecycle cleanly, not bypass it

4. The browser path is backend-first, not shell-first.
- first make `Plot2D V2` render through a WebGL2 backend
- only then add a browser host/shell on top
- do not promise full `LabV2` wasm parity before plot/backend partitioning is proven

## Current Baseline

- `FLabV2WindowManager` already owns:
  - workspace tabs
  - workspace-local panel visibility
  - default dock layouts
  - slab-window tiling/routing for plot hosts and monitor windows
- `FPlot2DWindowHostV2` already depends on `IPlotRenderBackendV2`
- desktop OpenGL rendering still uses the legacy OpenGL backend
- standalone wasm sandboxes already exist under `Studios/WebGL-WASM/`

## Migration Waves

### WN-00 Layout Contract Extraction

Status: implemented in this slice.

Move the default shell layout contract into shared `Slab` infrastructure:

- `FWorkspaceDefinitionV2`
- `FWorkspaceDockLayoutV2`
- `FDockNodeSplitV2`
- `FDockWindowPlacementV2`
- shared ImGui dock-layout application helper

Outcome:
- `LabV2` still owns panel contents and workspace semantics
- default layout structure is no longer buried in one monolithic window-manager function

### WN-01 Shared Workspace Shell

Status: implemented for the shared shell/state layer in this slice.

Target shared abstractions:

- `FWorkspaceShellV2`
  - active workspace
  - workspace tab strip
  - per-workspace visibility state
  - reset-to-default-layout flow
- `FPanelSurfaceRegistryV2`
  - deterministic panel registration and draw submission order
- `FWorkspaceRoutingPolicyV2`
  - whether a workspace hosts slab windows
  - slab-window hide/offscreen behavior when inactive

Move into `Slab`:
- top-tab rendering
- dockspace host lifetime
- per-workspace dock reset/bootstrap
- generic panel registration model
- shared workspace visibility persistence

Keep in `Studios/LabV2`:
- panel contents
- workspace-specific toggles and labels
- model/ontology/schemes behavior

Current implementation notes:
- shared shell state, workspace launcher, tab strip, workspace visibility strip, dockspace host, and panel-surface draw helpers now live in `Slab/Graphics/Window/V2/WorkspaceShellV2.*`
- `LabV2` now feeds the shared shell with workspace definitions, visibility items, and panel registrations instead of owning the dock/tab host logic inline
- workspace-specific launcher docking and slab-window routing still remain local to `LabV2`; those become the bridge into the hosted-surface wave

### WN-02 Hosted Surface Boundary

Status: initial `FHostedSurfaceV2` extraction implemented in this slice.

Direction lock:

- `FHostedSurfaceV2`
  - stable surface id / title
  - visibility hooks
  - focus-preparation hook
  - preferred workspace or routing scope
- `FSlabHostedSurfaceV2`
  - shared adapter around the current `FSlabWindow` path
- `FPlot2DWindowHostV2`
  - concrete `Plot2D V2` hosted surface
  - continues using `IPlotRenderBackendV2`

Constraint:
- this should be a wrapper/extraction of the existing `FSlabWindow` behavior, not a rewrite that forks the event/render pipeline

Current implementation notes:
- shared hosted-surface contracts now live in `Slab/Graphics/Window/V2/HostedSurfaceV2.*`
- `LabV2` plot-host routing now tracks hosted surfaces instead of raw plot-host windows for:
  - plot-host discovery/sync
  - hosted-surface visibility checks
  - hosted-surface focus preparation before bringing the slab window forward
- the underlying desktop render/input pipeline still runs through the existing `FSlabWindow` / `FPlot2DWindowHostV2` objects
- generic non-plot slab monitor windows still remain on the raw `FSlabWindow` path for now

### WN-03 Default Adoption

After the shell and hosted-surface APIs exist:

- make the shared shell the default for new editor-style studios
- keep specialized studies free to opt out
- migrate `LabV2` to consume the shared shell with only workspace/panel declarations left locally

## WebGL2 / WASM Renderer Plan

### WG-00 Keep The Existing Backend Seam

Do not route browser work through legacy `Lib/Graphics` desktop abstractions.

Shared seam:
- `FPlot2DWindowV2` produces `FPlotDrawListV2`
- `IPlotRenderBackendV2` consumes `FPlotFrameContextV2 + FPlotDrawListV2`

### WG-01 WebGL2 Backend Parity Slice

Add `FWebGL2RenderBackendV2` under `Slab/Graphics/Plot2D/V2/Backends/`.

Initial supported commands:

- filled/background rectangles
- polyline
- point sets
- screen-space text placeholder path

Non-goals for the first slice:

- NanoTeX integration
- full legacy OpenGL visual parity
- browser-hosted full `LabV2`

### WG-02 Browser Plot Host

Add a browser-safe plot host in the wasm sandbox area first:

- browser canvas sizing
- wasm event translation
- frame scheduling via Emscripten main loop
- one interactive `Plot2D V2` surface

This host should live in the browser-target path until dependency partitioning is stable.

### WG-03 Text Strategy

Browser-safe text needs a separate plan because desktop plot text currently depends on the native writer/theme stack.

First acceptable options:

- ImGui-rendered overlay text for HUD-only content
- a small atlas-backed text path for plot labels
- draw-list command subset that tolerates temporary label degradation

Do not block geometry/backend work on full math typesetting parity.

### WG-04 Browser Shell Expansion

Only after `WG-01..03` succeed:

- consider a browser `Plots` workspace
- then consider a reduced browser editor shell
- full `LabV2` shell remains a later decision gate

## Validation Gates

For windowing/layout waves:

- `cmake --build cmake-build-debug --target StudioSlab -j8`
- `./Build/bin/StudioSlab`
- workspace switching keeps docked windows attached
- slab-window workspaces continue routing plots only where intended

For WebGL2 waves:

- wasm sandbox target builds from clean checkout
- one `Plot2D V2` scene renders through `IPlotRenderBackendV2` in browser
- pointer pan/zoom/select works on that bounded plot surface

## Immediate Next Slice

1. Keep `WorkspaceLayoutV2` + `WorkspaceShellV2` as the shared default shell baseline.
2. Extract and widen the hosted-surface boundary (`FHostedSurfaceV2` / `FSlabHostedSurfaceV2`) around the current slab-window and plot-host lifecycle.
3. Preserve `LabV2` as the first consumer while reducing local routing/focus glue.
4. Follow with `IPlotRenderBackendV2` WebGL2 parity work in the wasm sandbox path.
