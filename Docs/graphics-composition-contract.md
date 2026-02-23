# Graphics Composition Contract (Current V1/V1.5)

This is a small internal contract note to keep the window/plot/GUI stack coherent while V2 monitor work advances.

## Scope

- `FPlatformWindow`
- `FSlabWindowManager`
- `FSlabWindow` composites (`FWindowPanel`, `FWindowColumn`, `FWindowRow`)
- nested `FGUIWindow` / `FPlot2DWindow`

## Render-Pass Rules

1. Composite windows must propagate both passes to children:
- `ImmediateDraw(...)`
- `RegisterDeferredDrawCalls(...)`

2. Window managers must render through `NotifyRender(...)` (or equivalent full-pass path), not only `ImmediateDraw(...)`.

3. Deferred GUI content (`FGUIWindow`, plot inspectors, ImGui overlays) is expected to be emitted in `RegisterDeferredDrawCalls(...)`.

## Layout / Visual Rules

1. Nested panes are **content panes**, not decorated top-level slab windows.
- Pane framing is container-owned (e.g. column/panel framing), not `FDecorator`-owned.

2. Container tiling must use symmetric insets/gaps (left/right and top/bottom), not one-sided gap subtraction.

3. Visual tests should verify composite layouts with mixed GUI + plot panes.

4. Visual apps/tests should prefer the shared host setup path (`Studios/Common/VisualHost.*`) so backend/GUI/window-manager initialization stays consistent.

## Current Visual Regression Coverage (`SlabTests`)

- `window-panel-gui` : deferred GUI propagation in a panel
- `panel-plot-and-gui` : mixed GUI + plot visibility in one composite
- `row-heavy-layout` : row pane framing/inset regression check (mixed GUI + plot)
- `column-heavy-panel` : pane framing/inset regression check (multi-column, multi-row)
- `spi-live-monitor-mock` : passive V2 monitor layout + live telemetry/plot updates

## Next Cleanup Targets (not required for current fixes)

- Unify container layout/padding logic (`FWindowRow` / `FWindowColumn` / `FWindowPanel`)
- Make frame pipeline explicit at host level (`BeginFrame`, canvas draw, GUI build, `EndFrame`)
- Separate plot canvas / plot scene / plot inspector responsibilities
