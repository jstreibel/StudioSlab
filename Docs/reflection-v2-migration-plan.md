# Reflection V2 Migration Plan (Adapter-First)

## Purpose

Define a low-risk migration from legacy interface exposure (`FInterface`/`FParameter`) to a V2 reflection contract suitable for LabV2 Schemes/Blueprints and future multi-surface exposure.

## Strategy

- Keep legacy behavior operational.
- Introduce V2 reflection as a parallel boundary layer.
- Migrate consumers to V2 reflection incrementally.
- Avoid dual-owner semantics for parameter state.

## Phase Gates

## Phase A - Contract Freeze (Done in design)

Deliverables:
- `Docs/reflection-v2-contract.md`
- enum and invocation baseline locked
- no code behavior changes

Exit criteria:
- contract accepted
- policy enums accepted
- no heavy CQRS split decision accepted

Status (2026-03-04): `done`

## Phase B - Read-Only Adapter Path

Deliverables:
- adapter maps legacy interfaces/parameters to V2 schema
- LabV2 Schemes read-only panel consumes V2 schema
- discovery, grouping, metadata rendering

Exit criteria:
- Schemes can enumerate interfaces/params/ops from adapter output
- no runtime invocation yet

Status (2026-03-04): `done`

Implemented:
- adapter + schema/invoke headers:
  - `Slab/Core/Reflection/V2/ReflectionTypesV2.h`
  - `Slab/Core/Reflection/V2/ReflectionCodecsV2.h`
  - `Slab/Core/Reflection/V2/ReflectionInvokeV2.h`
  - `Slab/Core/Reflection/V2/LegacyInterfaceAdapterV2.h`
- Schemes panel now renders catalog discovery in:
  - `Studios/LabV2/LabV2WindowManager.cpp`

## Phase C - Operation Invocation Path

Deliverables:
- operation invoke dispatcher (`Invoke`)
- first command/query operations wired in host/runtime boundaries
- thread/run-state policy checks enforced

Exit criteria:
- at least one command and one query invoked through V2 operation metadata
- structured operation errors surfaced in UI

Status (2026-03-04): `done`

Implemented:
- default per-interface operations:
  - `query.parameters`
  - `query.parameter.get`
  - `command.parameter.set`
  - `command.parameter.apply_pending`
- policy enforcement implemented in invoke path:
  - `ThreadAffinity`
  - `RunStatePolicy`
  - query `SideEffectClass=None` invariant check
- wired in both:
  - LabV2 Schemes panel (`Studios/LabV2/LabV2WindowManager.cpp`)
  - CLI reflection command (`Studios/CLI/main.cpp`)

## Phase D - Typed Complex Parameters

Deliverables:
- first complex type adapter (function descriptor path)
- UI editor path for that type in Schemes
- validation and codec baseline (`CLI` + `JSON` minimum)

Exit criteria:
- one non-scalar parameter type round-trips through schema + edit + apply path

Status (2026-03-04): `done`

Implemented:
- complex type id + CLI codec:
  - `slab.math.function.r_to_r`
- function descriptor decode/encode baseline:
  - `family`, `expr`, `domain_min`, `domain_max`
- Schemes editor supports descriptor editing and set/apply paths.
- adapter includes `RestartRequired` staging semantics for complex parameter slice.

## Phase E - Consumer Convergence

Deliverables:
- CLI generation/parsing migrated to reflection codec path (incremental)
- legacy direct option wiring reduced
- Blueprint/Schemes and CLI consume the same contract model

Exit criteria:
- one vertical slice uses one reflection contract across CLI and LabV2

Status (2026-03-04): `done`

Implemented vertical slice:
- common adapter + invoke + codec path is consumed by both:
  - `Studios/CLI/main.cpp` (`Studios reflect ...`)
  - `Studios/LabV2/LabV2WindowManager.cpp` (Schemes panel)
- run-state/thread-affinity checks execute in both surfaces through shared invoke helper.

## Workstream IDs

- `RV2-00`: Reflection V2 contract and indexed docs
- `RV2-01`: Read-only adapter and Schemes discovery
- `RV2-02`: Operation invocation runtime path
- `RV2-03`: Complex type adapter (functions)
- `RV2-04`: CLI convergence over reflection codecs

## Risk Register

- Legacy API dereference hazards for missing parameters.
- Metadata drift between adapter and runtime ownership semantics.
- Early over-generalization of type codecs before first complex slice lands.
- Hidden thread-affinity violations during operation invocation.

## Controls

- Enforce explicit null/error handling on missing parameter ids.
- Add schema validation checks before UI rendering or invocation.
- Keep first complex type scope narrow (`r_to_r` or `r2_to_r` function descriptor only).
- Gate operations by `ThreadAffinity` and `RunStatePolicy` before handler execution.

## Testing and Validation

Baseline for implementation phases:
- `cmake --build cmake-build-debug --target StudioSlab Studios testsuite -j8`
- `ctest --test-dir cmake-build-debug --output-on-failure`

Phase-specific:
- Phase B: Schemes discovery smoke in `StudioSlab`
- Phase C: operation invoke tests + UI error-path smoke
- Phase D: complex type serialization/deserialization tests
- Phase E: CLI parity checks against legacy behavior

## Documentation Sync Rule

When advancing any phase:
1. update `Docs/status-v2.md`
2. update `Docs/v2-feature-backlog.md`
3. update this migration plan with phase state
4. keep `Docs/index-reflection-v2.md` route accurate
