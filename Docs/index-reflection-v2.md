# Index: Reflection V2 (Interfaces, Parameters, Operations)

## Use This When

- Defining or evolving runtime reflection contracts (`Interface`, `Parameter`, `Operation`)
- Wiring LabV2 Schemes/Blueprints to reflected metadata
- Planning migration from legacy `FInterface`/`FParameter` to V2 reflection
- Aligning CLI/UI/API/language bindings over one shared interface model

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `SPEC.md` (V2 boundaries, migration policy)
3. `Docs/reflection-v2-decision-log.md`
4. `Docs/reflection-v2-contract.md`
5. `Docs/reflection-v2-migration-plan.md`
6. `Docs/reflection-v2-implementation.md`
7. Graph semantics/design direction when graph-surface changes are in scope:
   - `Docs/graph-onthology.md`
8. Focused quick path for Plot2D reflection:
   - `Docs/plot2d-reflection-v2-quick.md`
9. Registry-focused discovery/routing details:
   - `Docs/index-reflection-catalog-registry-v2.md`
   - `Docs/reflection-catalog-registry-v2-overview.md`
   - `Docs/reflection-catalog-registry-v2-routing.md`
10. Technical debt follow-up queue:
   - `Docs/technical-debt.md`
11. One planning context:
   - `Docs/v2-feature-backlog.md`
   - `Docs/v2-model-coverage-matrix.md`

## Core Code Map

- Reflection V2 runtime contract + adapter implementation:
  - `Slab/Core/Reflection/V2/ReflectionTypesV2.h`
  - `Slab/Core/Reflection/V2/ReflectionCodecsV2.h`
  - `Slab/Core/Reflection/V2/ReflectionInvokeV2.h`
  - `Slab/Core/Reflection/V2/ReflectionCatalogRegistryV2.h`
  - `Slab/Core/Reflection/V2/GraphSubstrateV2.h`
  - `Slab/Core/Reflection/V2/LegacyInterfaceAdapterV2.h`
- Legacy reflection seed (current):
  - `Lib/Core/Controller/Interface.h`
  - `Lib/Core/Controller/Interface.cpp`
  - `Lib/Core/Controller/Parameter/Parameter.h`
  - `Lib/Core/Controller/Parameter/BuiltinParameters.h`
  - `Lib/Core/Controller/CommandLine/CommandLineArgsManager.cpp`
- Legacy blueprint prototype reference:
  - `Studios/Lab/Blueprints/Blueprint.h`
  - `Studios/Lab/Blueprints/Blueprint.cpp`
  - `Studios/Lab/Blueprints/BlueprintRenderer.cpp`
- LabV2 Schemes insertion point:
  - `Studios/LabV2/LabV2WindowManager.cpp`
- Plot2D reflection catalog + legacy bridge:
  - `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.cpp`
  - `Slab/Graphics/Plot2D/V2/LegacyPlotReflectionAdapterV2.cpp`
- CLI reflection consumer:
  - `Studios/CLI/main.cpp`

## Current Baseline (Implemented, 2026-03-04)

- Unified operation model with `Kind = Command | Query`
- Single invocation path (no heavy CQRS split)
- Parameter policy includes `RestartRequired`
- Adapter-first migration: legacy remains source, V2 reflection is projected view
- First vertical slice is live in LabV2 Schemes + `Studios reflect` CLI

## Known Gaps / Agent Warnings

- `Blueprint Graph` is currently a visualization surface, not a full graph-execution editor.
- `Blueprint Graph` now uses a shared substrate document for nodes/edges/canvas, but execution/pin authoring is still out of scope.
- Graph node edits/layout are in-memory only; positions are not persisted across process restarts.
- Identifier styles are intentionally mixed:
  - legacy adapter ids keep snake_case compatibility
  - V2-first ids use kebab-case (for example `v2.function-sandbox`, `initial-condition`)
- Adapter output depends on legacy interfaces being registered in-process before catalog refresh.
  - Empty/partial catalog can be expected in startup phases where registration has not happened yet.
- Runtime operation path currently ships one complex type vertical slice (`slab.math.function.r_to_r`) via sandbox.
- Registry follow-ups and caveat TODOs are tracked in `Docs/technical-debt.md`.

## Validation Minimum

- Contract/document validation:
  - consistency across `Docs/status-v2.md`, this index, and `Docs/v2-feature-backlog.md`
- Build/smoke for current implementation:
  - `cmake --build cmake-build-debug --target StudioSlab Studios -j8`
  - `./Build/bin/Studios reflect --action list`
  - `./Build/bin/Studios reflect --action get --interface v2.function-sandbox --parameter initial-condition`
- Deeper validation:
  - `cmake --build cmake-build-debug --target testsuite -j8`
  - `ctest --test-dir cmake-build-debug --output-on-failure`
