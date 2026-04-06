# Reflection Catalog Registry V2: Routing

## Source Registration

`FLabV2WindowManager` registers two sources:

- `labv2.catalog.legacy`
  - backed by `FLegacyReflectionCatalogAdapterV2`
- `labv2.catalog.plots`
  - backed by `FPlotReflectionCatalogV2`

## Interface Routing Rule

- Registry maintains `InterfaceSourceById` index.
- `Invoke`/`EncodeCurrentParameterValue`:
  1. resolve `interfaceId` -> `sourceId`
  2. dispatch to that source callback
  3. return source result directly

## Merge Rule

- `BuildMergedCatalog()` unions interfaces by `InterfaceId`.
- Duplicate `InterfaceId` entries are skipped after first seen entry.
- Optional source tags are attached:
  - `catalog.source_id:<id>`
  - `catalog.source_name:<display>`

## UI Impact

- Schemes now sees all registered catalogs in one list.
- Schemes can execute operations from both legacy and plot sources via one invoke path.
