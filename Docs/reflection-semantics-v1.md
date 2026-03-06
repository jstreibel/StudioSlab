# Reflection Semantics V1

## Why an overlay (instead of changing ReflectionTypesV2 now)

Reflection V2 schemas (`FParameterSchemaV2`, `FOperationSchemaV2`, `FInterfaceSchemaV2`) are already used broadly in LabV2 and tests.  
`ReflectionSemanticsOverlayV1` attaches semantic meaning without changing those core structs:

- Parameter semantics: `(interface_id, parameter_id) -> space_id`
- Operation port semantics: `(interface_id, operation_id, field_id) -> space_id`
- Operation meaning: `(interface_id, operation_id) -> semantic_operator_id`

This keeps the existing reflection call path stable while enabling semantic queries.

## Semantic operator vs implementation binding

- `FSemanticOperatorSchemaV1` describes abstract math meaning (domain/codomain spaces, kind, declared properties, LaTeX).
- `FImplementationBindingV1` maps that meaning to concrete reflection operations (`target_interface_id`, `target_operation_id`).

This separation lets one semantic operator map to multiple concrete implementations (for different interfaces/backends).

## How to bind semantic operators to reflection operations

1. Initialize seeds:
   - `InitSemanticCatalogV1()`
2. Attach semantics from a reflection catalog:
   - `AttachSemanticsForCatalogV1(catalog, overlay, semanticCatalog, options)`
3. Or bind explicitly:
   - `AttachOperationAsImplementationOf(interfaceId, operationId, semanticOperatorId, constraints, costHints)`

Defaults:
- Signature matching is strict-by-default (`QueryOperatorsForSignature` returns only exact space matches).
- Detailed matching is available via `QueryOperatorMatchesForSignature`, which reports:
  - `ExactMatch`
  - `CompatibleButNeedsCoercion` (with suggested coercion operator ids)
  - `Incompatible` (with diagnostics)
- Configuration operations (`command.parameter.*`, `command.artist.set_*`, etc.) are excluded from semantic bindings by default.
  - They can be included only with explicit `FAttachSemanticsOptionsV1` opt-in.

## Why this helps future graphs

This model supports three graph layers without forcing a UI rewrite:

- Template/Blueprint graph:
  - Nodes use semantic operators and ports use semantic spaces.
- Runtime instance graph:
  - Nodes resolve to concrete reflection operations via bindings.
- Semantics/theory graph:
  - Spaces/operators/relations (inverse/adjoint/equivalence/coercion) can be queried independently.

LabV2 currently calls `SyncReflectionSemanticsOverlayV1(...)` when catalogs refresh, so semantic data is available for incremental UI features later.
