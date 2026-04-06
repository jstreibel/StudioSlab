# Plot2D V2 Migration Plan

## Purpose

This note turns the current Plot2D migration into an implementation-facing plan.

It is specifically about migrating legacy `FPlot2DWindow` behavior into V2 while preserving the core architectural decision already present in code:
- plot windows and artists own scene/data state
- renderers stay backend-specific and swappable
- host/UI/input behavior stays outside the artists and outside the render backend

## Current Architecture (Actual Code)

### 1. Scene / reflection layer

- `Slab/Graphics/Plot2D/V2/Plot2DWindowV2.*`
- `Slab/Graphics/Plot2D/V2/PlotArtistV2.*`
- `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.*`
- `Slab/Graphics/Plot2D/V2/LegacyPlotReflectionAdapterV2.*`

Current responsibilities:
- window identity, region, viewport, auto-fit flag
- artist slot ownership plus z-order
- backend-neutral draw-list emission
- artist hit-testing and input dispatch
- reflection descriptors for V2 and legacy windows/artists in one catalog

### 2. Render backend layer

- `Slab/Graphics/Plot2D/V2/Plot2DDrawListV2.*`
- `Slab/Graphics/Plot2D/V2/Plot2DRenderBackendV2.h`
- `Slab/Graphics/Plot2D/V2/Backends/OpenGLRenderBackendV2.*`
- `Slab/Graphics/Plot2D/V2/Backends/RecordingRenderBackendV2.*`

Current responsibilities:
- render draw commands only
- keep OpenGL details out of artists and out of `FPlot2DWindowV2`
- allow deterministic test validation with the recording backend

### 3. Host / UI / input layer

- `Slab/Graphics/Plot2D/V2/Plot2DWindowHostV2.*`

Current responsibilities:
- slab-window hosting in the `Plots` workspace
- pan / zoom / fit / aspect-lock camera behavior
- artist pointer and keyboard event forwarding
- viewport-attached toolbar/detail UI
- host-owned HUD layout publication (`FPlotHudLayoutV2`) for artist screen-space overlays

This layer is intentionally outside `FPlot2DWindowV2`.

## Legacy Baseline To Preserve

Legacy `FPlot2DWindow` currently bundles all of the following:
- artist-slot ownership and z-order
- built-in background / axis / labels / x-hair artists
- viewport-attached controls and attached detail panel
- pan / zoom / fit / auto-fit behavior
- artist visibility and z-order editing
- artist-specific GUI blocks
- direct OpenGL drawing in window/artist code

V2 should preserve the user-facing behavior, but not the legacy coupling.

## Migration Decisions

1. Do not move ImGui controls into `FPlot2DWindowV2` or `FPlotArtistV2`.
- Those remain scene/reflection objects.
- Plot UI belongs to the host layer.

2. Let the host publish layout constraints instead of letting artists guess the corners.
- Screen-space artist HUDs should anchor through `FPlotFrameContextV2::HudLayout`.
- Artists should not query ImGui widget geometry directly.

3. Let the frame context publish text metrics instead of letting artists guess HUD spacing.
- Screen-space HUD cards should size from `FPlotFrameContextV2::TextMetrics`.
- Artists should not hardcode line heights / padding from one tuned font size.

4. Do not move backend calls back into artists.
- Artists emit commands only.
- Backends interpret commands.

5. Use reflection as the generic artist-control surface.
- In legacy, artist GUI is `HasGUI()` / `DrawGUI()`.
- In V2, generic controls should come from reflected parameters/operations where possible.

6. Keep the legacy reflection bridge until artist coverage is sufficient.
- Mixed discovery is already in place and should remain the compatibility path.

## Legacy vs V2 Gap Map

### Done

- backend-decoupled V2 draw path
- V2 reflection catalog
- legacy + V2 mixed plot discovery
- artist hit-testing and keyboard event dispatch
- first interactive built-in V2 artist (`ModelSemanticGraphArtistV2`)
- LabV2 V2-plot host for pan/zoom/render
- viewport-attached toolbar/detail controls on the V2 host
- host-to-artist HUD layout contract for screen-space overlays
- frame-published text metrics for font-driven screen-space HUD sizing
- V2 artist z-order metadata and mutation path

### Remaining

- port the remaining legacy-built-in artist roles:
  - labels / overlay text
  - x-hair / cursor readout
  - history
  - `R2toR`
- decide whether any artist still needs custom non-reflection GUI
- unify export/save behavior for non-OpenGL backends

## Phased Plan

### `PLOT-00` Foundation

Status: done

- V2 scene/draw/backend split
- reflection descriptors for V2 entities
- recording backend tests
- legacy reflection bridge

### `PLOT-01` Host Parity Pass

Status: done

- keep V2 host-side pan/zoom behavior separate from the window/artists
- add viewport-attached toolbar/detail controls in LabV2
- restore artist visibility and z-order editing on V2 windows
- expose V2 artist z-order through reflection/catalog output

### `PLOT-02` Core Artist Coverage

Status: next

- add V2 equivalents for the remaining legacy artists used by monitor windows
- preserve slot/z-order semantics
- avoid backend leakage in ports

### `PLOT-03` Shared Host Extraction

Status: done

- move `FPlot2DWindowHostV2` out of `LabV2WindowManager.cpp`
- keep LabV2 responsible only for workspace orchestration and discovery
- make the host reusable by non-Lab studios/tests

### `PLOT-04` Interaction Expansion

Status: next

- formalize artist drag semantics beyond click/hover/keyboard
- add explicit capture/release conventions for interactive artists
- add more than one real interactive artist, not only semantic graph

### `PLOT-05` Export / backend cleanup

Status: later

- make save/export independent from one concrete backend where practical
- keep host export behavior compatible while more backends are added

## Current Non-Ambiguous Implementation Boundary

The current codebase already resolves the most important architectural ambiguity:

- renderer code belongs in backends
- plot-scene state belongs in `FPlot2DWindowV2` / `FPlotArtistV2`
- plot UX belongs in the host layer

That boundary is strong enough to continue implementation without a redesign first.

## Known Gaps / Constraints

- V2 attached plot controls now live in shared plot infrastructure, but only the current slab-window host path is implemented.
- Screen-space overlay harmony now depends on artists using `FPlotFrameContextV2::HudLayout` instead of hardcoded viewport corners.
- Screen-space HUD sizing should now depend on `FPlotFrameContextV2::TextMetrics` instead of hardcoded line-height/width guesses.
- Reflection-driven overlay controls now cover the `R2Section` per-section visibility/style path cleanly, but more exotic editor widgets can stay in the plot inspector until needed.
- Legacy `LabelsArtist` and `XHairArtist` behavior is not yet ported into V2-native artists.
- `History` / `R2toR` remain blocked on the texture-backed field-artist migration path rather than simple polyline draw-list ports.

## Validation

- Build: `cmake --build cmake-build-debug --target testsuite StudioSlab -j8`
- Tests: `./Build/bin/testsuite "[Plot2DV2]"`
