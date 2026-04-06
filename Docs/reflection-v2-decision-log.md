# Reflection V2 Decision Log

## Purpose

Short, date-stamped log of reflection/runtime-interface decisions that agents should treat as locked unless explicitly superseded.

Use this with:
- `Docs/index-reflection-v2.md`
- `Docs/reflection-v2-contract.md`
- `Docs/status-v2.md`

## Format Rule

For each entry:
- `Date`
- `Decision`
- `Scope`
- `Rationale`
- `Implications`
- `Supersedes` (optional)

## Decisions

### 2026-03-04 — Unified Operation Model (No Heavy CQRS Split)

- Decision:
  - Keep one operation model with `Kind = Command | Query` metadata.
  - Keep one invoke path for both (`Invoke`).
- Scope:
  - Reflection runtime contract, Schemes UI, CLI reflection path.
- Rationale:
  - Preserve operational clarity and low migration complexity.
  - Avoid architecture overhead from a full CQRS split at current maturity.
- Implications:
  - Command/query branching is metadata-driven, not architecture-tier driven.
  - Runtime policy enforcement remains centralized in invoke checks.

### 2026-03-04 — `RestartRequired` Staging Contract

- Decision:
  - Parameters marked `RestartRequired` stage edits and apply only on explicit apply/restart boundary.
- Scope:
  - Parameter editing in Schemes/CLI, adapter mutation path.
- Rationale:
  - Prevent false expectation that runtime state changed immediately.
- Implications:
  - UI must show staged vs live distinction.
  - Adapter keeps pending state in-process for current baseline.

### 2026-03-04 — Identifier Style Policy (Legacy vs V2)

- Decision:
  - Keep legacy adapter ids snake_case compatible.
  - Use kebab-case for V2-first ids.
- Scope:
  - Reflection ids exposed in UI/CLI (`interface`, `parameter`, `operation` ids).
- Rationale:
  - Preserve legacy compatibility while aligning new surfaces with V2 style direction.
- Implications:
  - Mixed-id environments are expected during migration.
  - Follow-up alias policy should be tracked as backlog item (`RV2-05`).

### 2026-03-04 — Schemes Surface Split

- Decision:
  - Separate reflection metadata/editor surface from blueprint graph surface:
    - `Interface Inspector`
    - `Blueprint Graph`
- Scope:
  - LabV2 Schemes workspace and default dock layout.
- Rationale:
  - Remove semantic overload from prior `Blueprints` naming and clarify purpose.
- Implications:
  - Current graph is visualization/navigation, not full execution graph authoring.
  - Execution interactions and persistence are follow-up work (`LAB-10`, `LAB-11`).

### 2026-03-04 — First Complex Type Baseline

- Decision:
  - Ship one complex type vertical slice first: `slab.math.function.r_to_r`.
- Scope:
  - Reflection codec and editor path.
- Rationale:
  - Validate codec/editor/invoke end-to-end before broadening type system.
- Implications:
  - Other complex types remain future work until explicitly added.

### 2026-03-04 — Adapter Registration Dependency

- Decision:
  - Accept that reflection catalog completeness depends on legacy interface registration timing.
- Scope:
  - Adapter refresh behavior in runtime sessions.
- Rationale:
  - Adapter-first migration keeps legacy registration lifecycle as source of truth.
- Implications:
  - Empty/partial catalogs can occur at startup phases.
  - Consumers should handle no-interface states explicitly.
