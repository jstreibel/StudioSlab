# Index: Reflection Catalog Registry V2

## Use This When

- Adding a new reflection catalog source.
- Debugging Schemes catalog discovery/routing in LabV2.
- Understanding how invoke/value reads are routed across legacy and plot catalogs.

## Read Order (Low Context)

1. `Docs/reflection-catalog-registry-v2-overview.md`
2. `Docs/reflection-catalog-registry-v2-routing.md`
3. `Docs/reflection-catalog-registry-v2-source-wiring.md`
4. `Docs/technical-debt.md` (Registry section)

## Code Map

- Registry contract/runtime:
  - `Slab/Core/Reflection/V2/ReflectionCatalogRegistryV2.h`
- LabV2 source registration + Schemes merged-catalog consumer:
  - `Studios/LabV2/LabV2WindowManager.h`
  - `Studios/LabV2/LabV2WindowManager.cpp`
- Plot catalog provider:
  - `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.h`
  - `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.cpp`
- Legacy catalog provider:
  - `Slab/Core/Reflection/V2/LegacyInterfaceAdapterV2.h`

## Snapshot (2026-03-05)

- Implemented: central registry for catalog-source discovery.
- Implemented: Schemes inspector + blueprint graph use merged catalog across all sources.
- Implemented: interface-scoped invoke/value routing through registry.
- Not yet unified: Plots inspector still calls plot adapter directly.
