# Index: Docs Structure and Roles

## Use This When

- You are not sure which doc to read first
- You need to update docs after a code change
- You want to separate status, plans, reference intent, handoffs, and debt
- You are cold-starting as an AI agent and want the shortest reliable route

## Cold-Start Read Order

1. `AGENTS.md`
2. `Docs/index.md`
3. `Docs/status-v2.md`
4. one task-specific index (`Docs/index-*.md`)
5. if resuming an in-progress slice, `Docs/index-handoffs.md`

## Doc Categories

### Routers / Entry Points

- `Docs/index.md`
- `Docs/index-v2-runtime.md`
- `Docs/index-labv2.md`
- `Docs/index-roadmap.md`
- `Docs/index-testing.md`
- `Docs/index-reflection-v2.md`
- `Docs/index-reflection-catalog-registry-v2.md`

### Current Implementation Status

- `Docs/status-v2.md`

Use this first when you need to know what is already implemented.

### Plans / Active Next Work

- `Docs/v2-feature-backlog.md`
- `Docs/v2-model-coverage-matrix.md`
- `Docs/ode-realization-descent-plan.md`
- `Docs/*-v2-slice-scope.md`
- `Docs/platform-wasm-feasibility.md`

Use these to decide what should happen next, not as proof that it already happened.

### Reference / Design Intent

- `Docs/product-direction.md`
- `Docs/product-vocabulary.md`
- `Docs/domain-model-ux-surfaces-spec.md`
- `Docs/study-model-spec.md`
- `Docs/sequence-control-spec.md`
- `Docs/model-v2-semantic-environment.md`
- `Docs/graph-onthology.md`
- reflection contract / implementation docs under `Docs/reflection-*.md`

Use these to preserve intent, terminology, and boundaries.

### Handoffs / Restart Notes

- `Docs/index-handoffs.md`
- `Docs/handoff-ode-realization-rz03.md`

Use these when continuing an in-progress slice. They are restart aids, not the main source of truth.

### To-Dos / Tech Debt

- `Docs/technical-debt.md`
- open items in `Docs/v2-feature-backlog.md`

### Validation Logs

- `Docs/validation/*.md`

## Conflict Rule

When docs disagree:
1. trust `Docs/status-v2.md`
2. verify in code under `Slab/` and `Studios/LabV2/`
3. treat plans and handoffs as intent unless backed by code

## Update Rule

When implementation changes:
1. update `Docs/status-v2.md`
2. update the relevant task index
3. update the specific reference or plan doc if it now drifts
4. update or add a handoff only if the slice is likely to be resumed later
