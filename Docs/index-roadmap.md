# Index: V2 Roadmap and Planning

## Use This When

- Selecting next V2 implementation task
- Reconciling status across planning docs
- Updating migration priorities or model coverage strategy
- Preparing implementation-ready slices from design drafts

## Status Precedence

Use this order when docs disagree:
1. `Docs/status-v2.md` (implementation snapshot)
2. `Docs/v2-feature-backlog.md` (active planning queue)
3. `Docs/v2-model-coverage-matrix.md` (coverage strategy)
4. Slice-scoping docs (`Docs/*-v2-slice-scope.md`)
5. Product/direction drafts for long-term intent

## Core Planning Docs

- Current status snapshot: `Docs/status-v2.md`
- Backlog and active items: `Docs/v2-feature-backlog.md`
- Coverage breadth: `Docs/v2-model-coverage-matrix.md`
- Platform feasibility notes: `Docs/platform-wasm-feasibility.md`
- Active slice (current): `Docs/functional-minimization-v2-slice-scope.md`
- Product context:
  - `Docs/product-direction.md`
  - `Docs/domain-model-ux-surfaces-spec.md`
  - `Docs/study-model-spec.md`
  - `Docs/sequence-control-spec.md`

## Update Protocol

When a task transitions (`todo` -> `active` -> `done`):
1. update `Docs/status-v2.md`
2. update `Docs/v2-feature-backlog.md`
3. patch affected slice-scope doc(s)
4. patch coverage matrix if model status changed

## Entropy Control Rule

Keep at most:
- one north-star item active
- one coverage item active
- one editor/tooling item active
