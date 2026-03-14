# Plot2D V2 Scheme

## Goals

- Keep the legacy mental model: one plot window owns multiple artists.
- Keep render backends decoupled from both the window and the artists.
- Keep host/UI/input behavior out of the artists.
- Keep legacy and V2 plot paths discoverable together during migration.

## Architecture

### 1. Scene layer

- `FPlot2DWindowV2`
  - owns window id, title, region, viewport, auto-fit flag
  - owns artist slots and z-order
  - builds a backend-neutral draw list
  - dispatches artist hit-test / pointer / keyboard events
- `FPlotArtistV2`
  - emits draw commands only
  - exposes reflection parameters/operations
  - may participate in hit-testing and interaction

### 2. Draw-command layer

- `FPlotDrawListV2`
  - `FPolylineCommandV2`
  - `FPointSetCommandV2`
  - `FRectangleCommandV2`
  - `FTextCommandV2`
- `FPlotFrameContextV2`
  - carries plot region / viewport plus host-published `FPlotHudLayoutV2` anchors for screen-space HUD placement
  - carries `FPlotTextMetricsV2` so screen-space HUD sizing can follow the active writer/font metrics instead of hardcoded pixel guesses

Coordinate spaces:
- plot space
- screen space

### 3. Backend layer

- `IPlotRenderBackendV2`
  - `Render(frameContext, drawList)`
- current backends:
  - `FOpenGLRenderBackendV2`
  - `FRecordingRenderBackendV2`

### 4. Host layer

- shared host: `FPlot2DWindowHostV2` in `Slab/Graphics/Plot2D/V2/Plot2DWindowHostV2.*`

Responsibilities:
- slab-window hosting
- pan / zoom / fit / aspect-lock camera behavior
- legacy-style floating toolbar / right-edge strip plus attached detail UI
- publishing HUD anchor zones / safe area into the frame context so artist overlays avoid host chrome without learning about ImGui widget geometry
- forwarding pointer/keyboard events into the V2 scene

This is intentionally separate from `FPlot2DWindowV2`.

### 5. Reflection / migration layer

- `FPlotReflectionCatalogV2`
  - composed discovery across V2 and legacy plots
  - default parameter operations (`query.parameters`, `query.parameter.get`, `command.parameter.set`, `command.parameter.apply_pending`)
- `FLegacyPlotReflectionAdapterV2`
  - projects legacy `FPlot2DWindow` and `FArtist` into the same reflection catalog

## Current V2 Artist Set

- `FBackgroundArtistV2`
- `FAxisArtistV2`
- `FPointSetArtistV2`
- `FRtoRFunctionArtistV2`
- `FModelSemanticGraphArtistV2`
- `FR2SectionArtistV2`

## Implemented Interaction Baseline

- window-host pan / zoom / fit
- artist hit-testing
- artist pointer event dispatch
- artist keyboard event dispatch
- semantic-graph click activation
- semantic-graph keyboard controls
- legacy-style floating plot strips plus attached detail controls in LabV2 host
- host-published HUD anchors for artist screen-space overlays
- font-driven screen-space HUD sizing through frame text metrics

## Current Reflection Baseline

### Window

- `window_id`
- `title`
- `x_min`
- `x_max`
- `y_min`
- `y_max`
- `auto_fit_ranges`
- `artist_count`

Operations:
- `command.window.fit_to_artists`
- `query.window.list_artists`

### Artist common parameters

- `artist_id`
- `artist_type`
- `parent_window_id`
- `label`
- `visible`
- `affect_graph_ranges`
- `z_order`

## Migration Status

### Landed

- V2 scene/backend separation
- legacy + V2 mixed reflection discovery
- OpenGL backend adapter for V2 draw commands
- recording backend tests
- first interactive V2 artist
- host-side attached controls parity first slice
- shared host extraction out of `LabV2WindowManager.cpp`

### Still Missing

- labels/x-hair parity
- history / `R2toR` V2-native artists
- generic export story beyond the current OpenGL-host path

## File Map

- `Slab/Graphics/Plot2D/V2/Plot2DWindowV2.*`
- `Slab/Graphics/Plot2D/V2/PlotArtistV2.*`
- `Slab/Graphics/Plot2D/V2/Plot2DWindowHostV2.*`
- `Slab/Graphics/Plot2D/V2/Plot2DDrawListV2.*`
- `Slab/Graphics/Plot2D/V2/Plot2DRenderBackendV2.h`
- `Slab/Graphics/Plot2D/V2/PlotReflectionSchemaV2.h`
- `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.*`
- `Slab/Graphics/Plot2D/V2/LegacyPlotReflectionAdapterV2.*`
- `Slab/Graphics/Plot2D/V2/Artists/*ArtistV2.*`
- `Slab/Graphics/Plot2D/V2/Backends/*`
- `Lib/tests/test_plot2d_v2.cpp`

## Companion Doc

Implementation sequencing and remaining migration phases are tracked in:
- `Docs/plot2d-v2-migration-plan.md`
