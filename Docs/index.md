# Docs Index (AI Start Here)

## Purpose

Small entrypoint for AI agents and contributors. Route quickly to the smallest relevant doc set before touching code.

## AI Read Protocol (Minimum Context)

1. Read `AGENTS.md` for repository rules.
2. Read this file (`Docs/index.md`).
3. Pick exactly one sub-index below based on task type.
4. Read at most:
   - one contract/spec document
   - one roadmap/slice document
5. In the selected sub-index, always check `Known Gaps`/`Warnings` sections before coding.
6. If docs conflict:
   - trust `Docs/status-v2.md` first
   - then trust code in `Slab/` and `Studios/LabV2/`
   - treat older draft docs as potentially stale and update them after code changes

## Task Router

- Runtime/data/control implementation: `Docs/index-v2-runtime.md`
- Reflection/interfaces/operations migration: `Docs/index-reflection-v2.md`
- Reflection catalog registry specifics: `Docs/index-reflection-catalog-registry-v2.md`
- LabV2 UI/panel/launcher work: `Docs/index-labv2.md`
- Planning, priority, migration state: `Docs/index-roadmap.md`
- Build, tests, and smoke validation: `Docs/index-testing.md`

## Status Source Of Truth

- Current V2 implementation status: `Docs/status-v2.md`
- Keep date-stamped snapshots there. Other docs can carry design intent and longer rationale.

## Deep Specs (Read Only If Needed)

- Product/domain: `Docs/product-direction.md`, `Docs/product-vocabulary.md`, `Docs/domain-model-ux-surfaces-spec.md`, `Docs/study-model-spec.md`, `Docs/sequence-control-spec.md`
- V2 planning/slices: `Docs/v2-feature-backlog.md`, `Docs/v2-model-coverage-matrix.md`, `Docs/live-control-v2-spec.md`, `Docs/live-parameters-v2-slice-scope.md`, `Docs/kg-r2tor-v2-slice-scope.md`, `Docs/md-v2-slice-scope.md`, `Docs/functional-minimization-v2-slice-scope.md`
- Reflection: `Docs/index-reflection-v2.md`, `Docs/reflection-v2-decision-log.md`, `Docs/reflection-v2-contract.md`, `Docs/reflection-v2-migration-plan.md`, `Docs/reflection-v2-implementation.md`
- Reflection catalog registry deep-dive: `Docs/index-reflection-catalog-registry-v2.md`, `Docs/reflection-catalog-registry-v2-overview.md`, `Docs/reflection-catalog-registry-v2-routing.md`, `Docs/reflection-catalog-registry-v2-source-wiring.md`
- Plot2D reflection quick path: `Docs/plot2d-reflection-v2-quick.md`
- Platform feasibility: `Docs/platform-wasm-feasibility.md`
- Runtime/UI contracts: `Docs/graphics-composition-contract.md`, `Docs/plot-window-ux-contract.md`, `Docs/slab-root-migration-policy.md`
- Monitoring architecture (LiveView vs listeners): `Docs/monitoring-liveview-vs-listeners.md`
- Validation logs: `Docs/validation/imgui-blueprints-smoke.md`, `Docs/validation/imgui-upstream-parity-log.md`
- Technical debt TODO backlog: `Docs/technical-debt.md`

## Maintenance Rule

When implementation status changes:
1. update `Docs/status-v2.md`
2. update the relevant index file
3. update detailed specs if they now drift from reality
