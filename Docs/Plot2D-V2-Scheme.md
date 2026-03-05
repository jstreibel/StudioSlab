# Plot2D Window V2 Scheme

## Goals
- Keep Plot Window behavior familiar: a window owns multiple artists and draws 2D content.
- Decouple rendering backend from both window and artist logic.
- Make Plot Window and artists reflectable through Reflection V2 schemas and operations.
- Keep migration incremental: V1 and V2 can coexist.

## Non-goals (current iteration)
- Full parity with every legacy artist type in one pass.
- Immediate replacement of `FPlot2DWindow` legacy runtime path.
- Locking into one graphics API (OpenGL/Vulkan/D3D/WebGL/Cairo).

## Architecture

### 1) Scene Model Layer (Backend-Agnostic)
- `FPlot2DWindowV2`
  - Owns window identity, title, region, viewport, and artist slots with z-order.
  - Builds a backend-neutral draw list from artists.
  - Supports fit-to-artists range review (similar legacy auto-range behavior).
- `FPlotArtistV2`
  - Base class for artists.
  - Emits draw commands, never direct API calls.
  - Exposes reflection descriptor fragments.

### 2) Draw Command Layer
- `FPlotDrawListV2`
  - Collects draw commands and legend entries.
- Command types:
  - `FPolylineCommandV2`
  - `FPointSetCommandV2`
  - `FRectangleCommandV2`
  - `FTextCommandV2`
- Coordinate spaces:
  - Plot space (data coordinates)
  - Screen space (pixel/UI overlays)

### 3) Rendering Backend Layer
- `IPlotRenderBackendV2`
  - Single backend contract: `Render(frameContext, drawList)`.
  - Backends can target legacy OpenGL, modern OpenGL, Vulkan, Direct3D, WebGL bridge, Cairo, or software.
- Implemented in this iteration:
  - `FRecordingRenderBackendV2` (test backend) for deterministic verification of command emission.

### 4) Reflection Layer
- `IPlotReflectableEntityV2`
  - Shared reflection surface for windows and artists.
- Window/artist reflection descriptors:
  - Parameters with typed schemas, mutability, exposure, and current/set handlers.
  - Operations for both generic parameter commands and entity-specific commands.
- `FPlotReflectionCatalogV2`
  - Discovers live windows/artists.
  - Builds `FReflectionCatalogV2`.
  - Exposes invocation path with Reflection V2 operation guardrails.
  - Provides default ops equivalent to existing adapter patterns:
    - `query.parameters`
    - `query.parameter.get`
    - `command.parameter.set`
    - `command.parameter.apply_pending`

## Artist Set in V2 (current)
- `FAxisArtistV2`
  - Grid + principal axes, with reflected toggles and tick density.
- `FPointSetArtistV2`
  - Point/polyline rendering with reflected style controls.
- `FRtoRFunctionArtistV2`
  - Functional sampling over domain with reflected sample/domain parameters.

## Reflection Contract Details

### Window Parameters (initial set)
- `window_id` (const)
- `title` (runtime mutable)
- `x_min`, `x_max`, `y_min`, `y_max` (runtime mutable)
- `auto_fit_ranges` (runtime mutable)
- `artist_count` (const)

### Window Operations
- `command.window.fit_to_artists`
  - Optional `padding_fraction` input.
  - Outputs fitted flag and resulting bounds.
- `query.window.list_artists`
  - Returns current artist ids and count.

### Artist Common Parameters
- `artist_id` (const)
- `artist_type` (const)
- `label` (runtime mutable)
- `visible` (runtime mutable)
- `affect_graph_ranges` (runtime mutable)

## Migration Strategy

### Phase 1 (implemented)
- Introduce V2 side-by-side under `Slab/Graphics/Plot2D/V2`.
- Keep legacy classes unchanged.
- Use test backend to validate behavior and reflection semantics.

### Phase 2
- Add runtime backends (legacy GL adapter first for immediate visual parity).
- Port additional artists (`R2toR`, sections, history, overlays).
- Add legacy-to-V2 reflection bridge so legacy windows/artists are discoverable in one catalog.

### Phase 3
- Integrate V2 reflection catalog composition in Lab/CLI surfaces.
- Add compatibility shims for old plotter APIs where needed.

### Phase 4
- Decommission direct OpenGL calls from artist logic.
- Keep backends swappable per window/context.

## File Map (this iteration)
- `Slab/Graphics/Plot2D/V2/Plot2DWindowV2.*`
- `Slab/Graphics/Plot2D/V2/PlotArtistV2.*`
- `Slab/Graphics/Plot2D/V2/Plot2DDrawListV2.*`
- `Slab/Graphics/Plot2D/V2/Plot2DRenderBackendV2.h`
- `Slab/Graphics/Plot2D/V2/PlotReflectionSchemaV2.h`
- `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.*`
- `Slab/Graphics/Plot2D/V2/LegacyPlotReflectionAdapterV2.*`
- `Slab/Graphics/Plot2D/V2/Artists/*ArtistV2.*`
- `Slab/Graphics/Plot2D/V2/Backends/RecordingRenderBackendV2.*`
- `Slab/Graphics/Plot2D/V2/Plot2DV2.h`

## Validation
- Added Catch2 coverage in `Lib/tests/test_plot2d_v2.cpp`:
  - Draw command emission through backend abstraction.
  - Fit-to-artists bounds behavior.
  - Reflection query/set and window command execution.
