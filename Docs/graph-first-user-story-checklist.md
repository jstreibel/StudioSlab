# Graph First User Story Checklist

**Status:** execution checklist (`2026-03-06`)
**Purpose:** drive a concrete first graph workflow that validates StudioSlab graph ontology end-to-end.

## 1) First User Story

As a simulation user, I can:
1. compose a small **Template graph** from semantic operators,
2. instantiate it into a **Runtime graph** instance,
3. use **Routing graph** to connect a runtime output endpoint to a viewer/input endpoint,
4. observe immediate live effect and clear diagnostics.

This is the first story because it proves the intended mode pipeline:
`Semantic/Template -> Runtime -> Routing`.

## 2) Outcome Checklist (Acceptance)

- [x] A minimal template can be authored from semantic operator nodes and value-flow edges.
- [x] Template compile/instantiate action exists (mock allowed for first slice) and produces a runtime instance graph payload.
- [x] Instantiation result surfaces bound/unbound nodes and edge diagnostics in UI.
- [x] Runtime graph can show the instantiated instance (not only reflection catalog projection).
- [x] Routing graph can list/select runtime endpoints from the instance payload.
- [x] A routing connect action is executed through an operation path (not only local edge drawing).
- [x] On successful connect, user sees immediate observable feedback (status + visual/telemetry effect).
- [x] On failed connect, user sees actionable diagnostics (policy/type/space mismatch).
- [x] Edge meaning remains explicit (`ValueFlow`, `HandleBinding`, `StreamSubscription`, `ControlDependency`).
- [x] Graph-triggered operations respect centralized policy enforcement.

## 3) Build Checklist (Implementation)

### A. Template -> Runtime Instantiation Slice
- [x] Define a minimal instantiation artifact (instance id + instance nodes + ports + edges + diagnostics).
- [x] Add one explicit instantiation action in Graph Playground.
- [x] Use existing semantic operator bindings; for unbound operators emit diagnostics, not silent skips.
- [x] Preserve node ids/port ids in a traceable way (`template node id -> runtime node id` mapping).

### B. Runtime Instance Surface
- [x] Add a runtime panel mode/toggle for instantiated graph view.
- [x] Keep reflection snapshot mode available; do not regress existing inspection flow.
- [x] Show instance status and compile/instantiation diagnostics near graph.

### C. Routing Integration
- [x] Provide endpoint source from instantiated runtime graph.
- [x] Connect uses an invocation path with status result.
- [x] Routing edge creation reflects actual connect result (success/fail), not optimistic local mutation.

### D. UX Contracts
- [x] Right-click canvas add-node menu available where editing is allowed.
- [x] Pin hover tooltip shows id/direction/space/type hints.
- [x] Drag from pin -> drop -> contextual connect suggestions.
- [x] Read-only graph modes clearly indicate why add/connect actions are disabled.

### E. Persistence
- [x] Persist instantiation-related UI/session state needed for repeatability.
- [x] Persist routing endpoint metadata required to restore graph session.

### F. Validation
- [ ] Add one headless/unit test for template instantiation mapping.
- [ ] Add one integration smoke for routing connect success/failure reporting.
- [ ] Manual LabV2 smoke: create template, instantiate, route, observe effect, reconnect/disconnect.

## 4) Definition of Done

- [x] User can complete the story in one session without hidden/manual state edits.
- [x] Diagnostics are visible at each phase (compile, instantiate, connect).
- [x] No ambiguity about edge semantics in UI or persisted state.
- [x] Existing Schemes and Graph Playground surfaces continue to build and run.
