# Index: Reflection V2 (Interfaces, Parameters, Operations)

## Use This When

- Defining or evolving runtime reflection contracts (`Interface`, `Parameter`, `Operation`)
- Wiring LabV2 Schemes/Blueprints to reflected metadata
- Planning migration from legacy `FInterface`/`FParameter` to V2 reflection
- Aligning CLI/UI/API/language bindings over one shared interface model

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `SPEC.md` (V2 boundaries, migration policy)
3. `Docs/reflection-v2-contract.md`
4. `Docs/reflection-v2-migration-plan.md`
5. `Docs/reflection-v2-implementation.md`
6. One planning context:
   - `Docs/v2-feature-backlog.md`
   - `Docs/v2-model-coverage-matrix.md`

## Core Code Map

- Reflection V2 runtime contract + adapter implementation:
  - `Slab/Core/Reflection/V2/ReflectionTypesV2.h`
  - `Slab/Core/Reflection/V2/ReflectionCodecsV2.h`
  - `Slab/Core/Reflection/V2/ReflectionInvokeV2.h`
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
- CLI reflection consumer:
  - `Studios/CLI/main.cpp`

## Current Baseline (Implemented, 2026-03-04)

- Unified operation model with `Kind = Command | Query`
- Single invocation path (no heavy CQRS split)
- Parameter policy includes `RestartRequired`
- Adapter-first migration: legacy remains source, V2 reflection is projected view
- First vertical slice is live in LabV2 Schemes + `Studios reflect` CLI

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
