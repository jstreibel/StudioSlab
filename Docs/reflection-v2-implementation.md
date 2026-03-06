# Reflection V2 Implementation Notes (RV2-01..RV2-04)

## Scope (2026-03-06)

Implemented vertical slice across LabV2 Schemes and CLI using one shared reflection contract and invoke path.

## Code Map

Shared reflection runtime (header-only):
- `Slab/Core/Reflection/V2/ReflectionTypesV2.h`
- `Slab/Core/Reflection/V2/ReflectionCodecsV2.h`
- `Slab/Core/Reflection/V2/ReflectionInvokeV2.h`
- `Slab/Core/Reflection/V2/GraphSubstrateV2.h`
- `Slab/Core/Reflection/V2/LegacyInterfaceAdapterV2.h`

LabV2 Schemes consumer:
- `Studios/LabV2/LabV2WindowManager.h`
- `Studios/LabV2/LabV2WindowManager.cpp`

CLI consumer:
- `Studios/CLI/main.cpp`

## Implemented Contract Path

Adapter source:
- legacy `FInterface` / `FParameter` from `FInterfaceManager`

Adapter outputs:
- `FReflectionCatalogV2`
- per-interface `FParameterSchemaV2` and `FOperationSchemaV2`

Default operations per interface:
- `query.parameters`
- `query.parameter.get`
- `command.parameter.set`
- `command.parameter.apply_pending`

Policy checks enforced before handler execution:
- `ThreadAffinity`
- `RunStatePolicy`
- query side-effect invariant (`Query => SideEffectClass=None`)

## Complex Type Slice

Complex type implemented:
- `TypeId = slab.math.function.r_to_r`

Codec shape (CLI baseline):
- `family=<...>;expr=<...>;domain_min=<...>;domain_max=<...>`

Sandbox interface exposed by adapter for first round-trip:
- interface id: `v2.function-sandbox`
- parameter id: `initial-condition`
- mutability: `RestartRequired`

`RestartRequired` behavior implemented:
- set command stages pending value
- apply command commits pending value

## Schemes Panel Behavior

Under `Schemes` workspace:
- `Interface Inspector` panel:
- catalog discovery (interfaces, parameters, operations)
- operation invocation buttons
- parameter table with live value read
- parameter editor (scalar + function descriptor)
- set/apply actions with structured result output
- `Blueprint Graph` panel:
- node-canvas view for selected interface
- parameter and operation nodes linked through interface node (rendered from substrate edges)
- node drag + canvas pan + grid + auto-layout reset controls
- shared graph substrate document for nodes/edges/canvas in Schemes surface
- graph interactions currently drive selection/navigation, not operation execution wiring

## CLI Behavior

New command:
- `Studios reflect`

Supported actions:
- `--action list`
- `--action get --interface <id> --parameter <id>`
- `--action set --interface <id> --parameter <id> --value <encoded>`
- `--action apply --interface <id>`
- `--action invoke --interface <id> --operation <id> --arg key=value`

Invocation context flags:
- `--thread any|ui|simulation|worker`
- `--running`

## Validation Performed

Build:
- `cmake --build cmake-build-debug --target StudioSlab Studios -j8`

CLI smoke:
- reflection list/get/set/invoke paths
- run-state policy violation path (`StoppedOnly` op invoked with `--running`)

## Known Limitations

- Adapter refresh is in-memory; staged values are not persisted across process restarts.
- Complex parameter is currently sandbox-scoped (first typed round-trip baseline).
- Legacy CLI command definitions for simulation slices are still direct/manual (full convergence remains incremental).
- `Blueprint Graph` is not yet a full blueprint authoring/runtime surface (no pin-to-pin invoke wiring yet).
- Graph node layout is currently process-local and non-persistent.
- Identifier style is mixed by design (`legacy.*` snake_case compatibility vs V2 kebab-case ids).
- Catalog completeness depends on legacy interface registration timing within the running process.
