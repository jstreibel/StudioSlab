# Plot2D Reflection V2 Quick Guide

## Goal

Expose both Plot2D V2 and legacy Plot2D (`FPlot2DWindow` + `FArtist`) through one Reflection V2 catalog.

## Read This First (Low Entropy)

1. `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.cpp`
2. `Slab/Graphics/Plot2D/V2/LegacyPlotReflectionAdapterV2.cpp`
3. `Slab/Graphics/Plot2D/V2/Plot2DWindowV2.cpp`
4. `Studios/LabV2/LabV2WindowManager.cpp` (`DrawPlotsInspectorPanel`)

## Interface Id Patterns

- V2 window: `v2.plot.window.<window_id>`
- V2 artist: `v2.plot.artist.<window_id>.<artist_id>`
- Legacy window bridge: `legacy.plot.window.<legacy_window_id>`
- Legacy artist bridge: `legacy.plot.artist.<legacy_window_id>.<artist_id>`

## Legacy Hooks Required By Bridge

- Window discovery: `FPlot2DWindow::GetLiveWindows()`
- Stable window id: `FPlot2DWindow::GetStableWindowIdV2()`
- Artist slots + z-order: `FPlot2DWindow::GetArtists()`, `TryGetArtistZOrder`, `SetArtistZOrder`
- Artist metadata/params: `FArtist::DescribeV2Parameters`, `TryGetV2ParameterValue`, `TrySetV2ParameterValue`

## Thread Policy

- Legacy default parameter operations are marked `UI` thread affinity.
- V2-first operations remain `Any` unless operation-specific code says otherwise.

## Add New Legacy Artist Parameters

1. Override `DescribeV2Parameters` in the legacy artist.
2. Override `TryGetV2ParameterValue` and `TrySetV2ParameterValue`.
3. Keep values CLI-encoded strings and type ids aligned with `ReflectionCodecsV2.h`.

## Validation

- Build: `cmake --build cmake-build-debug --target testsuite -j8`
- Test: `ctest --test-dir cmake-build-debug --output-on-failure -R Plot2DV2`
