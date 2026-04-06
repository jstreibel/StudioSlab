# Reflection V2 Contract (Draft v0.1, Phase A Locked)

## Purpose

Define a stable, transport-neutral reflection contract for runtime interfaces in StudioSlab V2.

This contract is the shared source for:
- LabV2 Schemes/Blueprint surfaces
- CLI exposure/parsing codecs
- future HTTP/API and language bindings

## Scope

In scope:
- interface/parameter/operation schemas
- operation invocation contract
- parameter and operation policies
- type identifiers and value representation baseline
- adapter boundary from legacy `FInterface`/`FParameter`

Out of scope:
- graph execution runtime semantics
- persistence format finalization
- network transport specifics
- replay/CQRS architecture frameworks

## Design Rules

1. Reflection is not CLI-shaped.
2. CLI is one codec over reflection, not the owner of reflection.
3. Use one operation model and one invocation path.
4. Separate metadata contracts from concrete runtime object ownership.
5. Migrate adapter-first; preserve legacy behavior during transition.

## Schema Objects

### `FReflectionCatalog`

Root discovery container for exposed interfaces.

Required fields:
- `CatalogVersion`
- `Interfaces` (`Vector<FInterfaceSchema>`)

### `FInterfaceSchema`

Required fields:
- `InterfaceId` (stable machine id)
- `DisplayName`
- `Description`
- `Version`
- `Parameters` (`Vector<FParameterSchema>`)
- `Operations` (`Vector<FOperationSchema>`)
- `Children` (sub-interface descriptors or ids)
- `Tags`

### `FParameterSchema`

Required fields:
- `ParameterId` (stable machine id)
- `DisplayName`
- `Description`
- `TypeId`
- `Mutability`
- `Exposure`
- `DefaultValue` (optional)
- `CurrentValueProvider` capability (optional)
- `SetValueHandler` capability (optional)
- `Constraints` (`min`, `max`, `step`, `allowedValues`, `unit`)
- `UIHints` (`controlType`, `group`, `precision`)
- `Codecs` capabilities (`CLI`, `JSON`, `Script`)

### `FOperationSchema`

Required fields:
- `OperationId` (stable machine id)
- `DisplayName`
- `Description`
- `Kind` (`Command` or `Query`)
- `Inputs`
- `Outputs`
- `ThreadAffinity`
- `RunStatePolicy`
- `SideEffectClass`
- `InvokeHandler`

## Policy Enums

### `EParameterMutability`

- `Const`: runtime read-only
- `RuntimeMutable`: runtime writable
- `RestartRequired`: writable intent accepted, applied on restart/rebuild boundary

### `EParameterExposure`

- `Hidden`
- `ReadOnlyExposed`
- `WritableExposed`

### `EOperationKind`

- `Command`
- `Query`

### `EThreadAffinity`

Execution placement/safety contract:
- `Any`
- `UI`
- `Simulation`
- `Worker`

### `ERunStatePolicy`

- `Any`
- `StoppedOnly`
- `RunningOnly`

### `ESideEffectClass`

Impact scope contract:
- `None`
- `LocalState`
- `TaskLifecycle`
- `IO`
- `External`

## Invocation Contract

Single invocation API for both operation kinds:
- `Invoke(operationId, inputMap, context) -> FOperationResult`

`FOperationResult`:
- `Status` (`Ok` | `Error`)
- `ErrorCode` (optional)
- `ErrorMessage` (optional)
- `OutputMap`
- optional telemetry (`LatencyMs`, `ExecutedThread`, `Warnings`)

## Query/Command Semantics (No Heavy CQRS Split)

Use one operation model with metadata constraints:
- Queries must declare `SideEffectClass=None`.
- Commands may declare non-`None` side effects.
- Runtime and UI can still branch behavior by `Kind`.

This preserves clarity without introducing a separate CQRS architecture stack.

## Type System Baseline

Use stable `TypeId` strings.

Initial set:
- `slab.scalar.bool`
- `slab.scalar.int32`
- `slab.scalar.float64`
- `slab.scalar.string`
- `slab.scalar.string_list`
- `slab.object.ref`
- `slab.math.function.r_to_r`
- `slab.math.function.r2_to_r`

Complex values are represented as typed descriptors (`TypeId + payload` via codecs), not raw pointer exchange.

## `RestartRequired` Contract

`RestartRequired` means:
- value edits are valid user intent
- running task/session behavior does not mutate immediately
- value is staged as pending configuration
- apply occurs at explicit restart/rebuild boundary

UI/host obligations:
- mark pending-restart state
- expose explicit apply/restart action
- avoid presenting staged value as already live

## Legacy Adapter Boundary

Phase A/B baseline:
- legacy remains source (`FInterface`, `FParameter`)
- adapters project legacy into V2 schema
- no behavior changes in legacy path required

Adapter targets:
- `FInterface` -> `FInterfaceSchema`
- `FParameter` -> `FParameterSchema`
- legacy sub-interfaces -> `Children`
- legacy command-line naming -> `CLI codec` hints

## Invariants

1. Missing identifiers must not cause invalid dereference in adapters/invokers.
2. Policy metadata must be explicit (no implicit runtime mutability).
3. Reflection ids should be stable across process runs and minor UI changes.
4. Operation invocation must return structured error data, not only logs.

## Open Issues (Phase A Deferred)

- final id normalization rules (`InterfaceId`, `ParameterId`, `OperationId`)
- value payload canonical shape for complex math functions
- permission model for remote/API invocations
- undo/redo semantics for command operations
