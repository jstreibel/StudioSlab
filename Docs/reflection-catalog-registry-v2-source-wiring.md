# Reflection Catalog Registry V2: Source Wiring

## LabV2 Wiring Points

- Register sources in `FLabV2WindowManager` constructor.
- Unregister sources in `FLabV2WindowManager` destructor.
- Use registry merged catalog in:
  - `DrawSchemesInspectorPanel()`
  - `DrawSchemesBlueprintGraphPanel()`

## Plot Workspace Host Sync

`SyncPlotWorkspaceWindows()` refreshes the plot source from registry and uses that source catalog to discover V2 plot window interfaces.

## Operational Notes

- Schemes operation input preparation is generic:
  - checks operation schema required fields (`parameter_id`, `value`)
  - fills inputs only when required by the selected operation
- Set/apply operation IDs are resolved from available operation schema, supporting both legacy and plot naming.
