# V2 Architecture Charter

## Purpose

This charter defines the intended architecture boundaries for V2 code.

Use it when deciding:
- where new code belongs
- which architectural pattern should govern a subsystem
- whether a dependency crosses the wrong boundary
- whether a LabV2 feature should stay app-local or move into `Slab/`
- whether a legacy dependency should be wrapped behind a V2 adapter

This document is a design contract, not an implementation status report.
For current implementation status, read `Docs/status-v2.md` first.

## Source-Of-Truth Order

When documents disagree:

1. `Docs/status-v2.md`
2. code under `Slab/`, `Studios/LabV2/`, and relevant `Lib/Models/*/V2`
3. this charter
4. focused design docs and slice plans

If implementation intentionally changes an architectural boundary, update this charter in the same change.

## 1. Charter Index

Stable IDs below are intended for code reviews, handoffs, and agent instructions.

| ID | Rule |
| --- | --- |
| `ARCH-V2-001` | StudioSlab V2 is stratified; do not force one architecture across the whole codebase. |
| `ARCH-V2-002` | Application targets are composition roots; reusable policy belongs in `Slab/` or `Lib/`, not in app shells. |
| `ARCH-V2-003` | Shared V2 platform/composition code uses ports/adapters plus explicit service/module registration. |
| `ARCH-V2-004` | Numerics V2 uses a recipe/session/task pipeline with scheduling separated from output behavior. |
| `ARCH-V2-005` | LiveData and LiveControl use topic/dataflow semantics; UI should consume snapshots or typed topics. |
| `ARCH-V2-006` | Model V2 is DDD-inspired domain semantics plus compiler-style derived artifacts. |
| `ARCH-V2-007` | Reflection V2 is a capability/introspection layer, not the owner of domain state. |
| `ARCH-V2-008` | Graph V2 documents are projections by default; executable graphs must be explicit runtime artifacts. |
| `ARCH-V2-009` | Plot2D/visualization uses artists, draw lists, and render-backend strategy seams. |
| `ARCH-V2-010` | LabV2 is a workbench shell with controllers and panels, not a reusable infrastructure layer. |
| `ARCH-V2-011` | Shared simulation slices are vertical composition glue, not dumping grounds for generic runtime policy. |
| `ARCH-V2-012` | Legacy integration is an anti-corruption layer; legacy globals should not leak into V2 contracts. |
| `ARCH-V2-013` | Architecture changes require index/status/doc updates and targeted validation notes. |

## 2. Global Architecture Rule

There is no single correct architecture for the whole repository.

StudioSlab V2 should be treated as layered but not strictly three-tiered:

- domain semantics need DDD-style vocabulary
- numeric execution needs pipeline/task architecture
- live runtime state needs dataflow/topic architecture
- reflection needs capability metadata and invocation contracts
- visualization needs retained drawing documents and backend seams
- LabV2 needs immediate-mode workbench composition
- app targets need explicit composition roots

The common rule across all layers is orthogonality:

- construction is separate from execution
- execution is separate from observation
- observation is separate from control
- domain semantics are separate from projections
- platform hosting is separate from render/backend capability
- app orchestration is separate from reusable infrastructure

## 3. Canonical Strata

| Stratum | Primary Paths | Architecture | Owns | Must Not Own |
| --- | --- | --- | --- | --- |
| Composition / Platform | `Slab/Core/Composition/V2`, `Slab/Core/Platform/V2` | Ports/adapters, composition root support, service registry | runtime context, platform-host contracts, modules, typed services | ImGui panels, model semantics, solver loops, render-specific implementation details |
| Numerics Runtime | `Slab/Math/Numerics/V2` | Recipe/session/task pipeline, strategy, observer/listener | recipes, sessions, tasks, triggers, schedulers, listeners, dispatch policy | UI windows, ImGui, app-specific launchers, direct monitor widgets |
| Live Data / Control | `Slab/Math/Data/V2` | In-process topic/dataflow, latest snapshot delivery, typed control samples | live topics, hubs, snapshot/status/telemetry/control DTOs | task lifetime ownership, plotting policy, domain validation |
| Model V2 | `Slab/Core/Model/V2` | DDD-inspired semantic model, compiler-style realization pipeline | definitions, relations, expressions, assumptions, validation, semantic overview, realization descriptors, runtime artifacts | LabV2 panels, concrete windowing, global task-manager access |
| Reflection V2 | `Slab/Core/Reflection/V2` | Capability/introspection schema, command/query operation model | interface catalogs, parameter schemas, operation schemas, adapters, invocation validation | domain ownership, persistence, UI-specific state |
| Graph Substrate | `Slab/Core/Reflection/V2/GraphSubstrateV2.*` and graph-facing model/ontology projections | Projection/document architecture | graph documents, nodes, edges, canvas policy, projection metadata | hidden side effects, implicit runtime execution |
| Ontology V2 | `Slab/Core/Ontology/V2` | Read-only domain/reference projection | ontology bundles, filters, activation state, graph projections | model-authoring truth, runtime execution |
| Plot2D / Visualization | `Slab/Graphics/Plot2D/V2` | Artist model, draw-list document, render-backend strategy | artists, draw commands, plot windows, plot reflection bridge, render backends | simulation scheduling, model realization, app launch policy |
| Window / Workspace Shell | `Slab/Graphics/Window/V2` | Workbench shell primitives, layout recipes, hosted-surface contracts | workspace definitions, dock recipes, hosted surfaces, reusable shell drawing helpers | Lab-specific panel state, model-specific actions |
| Shared Simulation Slices | `Slab/Studios/Common/Simulations/V2`, `Slab/Studios/Common/Monitors/V2` | Vertical slice composition glue | model recipe wiring, common monitor wiring, shared launch bundles | generic runtime abstractions that belong under `Slab/Math` or `Slab/Core` |
| LabV2 | `Studios/LabV2` | Immediate-mode workbench shell, app controllers, panel composition | app-specific state, panels, workspace routing, target-local orchestration | reusable runtime policy, reusable domain services, generic platform contracts |
| CLI | `Studios/CLI` | Composition root plus command dispatcher | command parsing, runtime profile selection, command-specific orchestration | reusable runtime behavior, app-independent service implementation |
| Legacy Bridge | `Slab/Core/*/V2/*Legacy*`, bounded adapters elsewhere | Anti-corruption layer | translation between legacy APIs and V2 contracts | new V2 semantics defined in legacy terms |

## 4. Dependency Direction

Default dependency direction:

1. `Studios/*` targets compose and orchestrate.
2. `Slab/Studios/Common/*` provides reusable app-facing slices.
3. `Slab/Graphics/*`, `Slab/Math/*`, and `Slab/Core/*` provide shared V2 infrastructure.
4. `Lib/Models/*/V2` provides model-specific recipes and kernels.
5. legacy `Lib/*` APIs are reached through bounded adapters when possible.

Rules:

- `Slab/Math/Numerics/V2` must not depend on LabV2, ImGui, or concrete windowing.
- `Slab/Core/Model/V2` must not depend on LabV2, ImGui, or task-manager globals.
- `Slab/Math/Data/V2` should expose transport-neutral DTOs and topics.
- `Slab/Graphics/Plot2D/V2` may expose reflection/plot controls but must not own simulation execution.
- `Studios/LabV2` may depend on shared V2 systems, but reusable policy extracted from it should move to `Slab/`.
- legacy global services should be wrapped behind V2 services, adapters, or composition modules before becoming shared dependencies.

## 5. Architecture Selection Rules

When adding or moving code, first identify the owning concern:

| Concern | Default Architecture | Default Home |
| --- | --- | --- |
| Process/window/event-loop startup | Platform host / composition root | `Slab/Core/Platform/V2`, app target |
| Service/backend selection | Ports/adapters + service registry | `Slab/Core/Composition/V2` |
| Numeric model execution | Recipe/session/task pipeline | `Slab/Math/Numerics/V2`, `Lib/Models/*/V2` |
| Output timing | Trigger strategy | `Slab/Math/Numerics/V2/Scheduling` |
| Output behavior | Listener strategy | `Slab/Math/Numerics/V2/Listeners` |
| Runtime telemetry/snapshots | Live topic/dataflow | `Slab/Math/Data/V2` |
| Runtime parameter input | Live control topic/binding | `Slab/Math/Data/V2`, `Slab/Studios/Common/Simulations/V2` |
| Authored mathematical model | Domain model / semantic environment | `Slab/Core/Model/V2` |
| Model-to-runtime lowering | Compiler-style realization pipeline | `Slab/Core/Model/V2` |
| Interface metadata | Reflection catalog/source/invocation | `Slab/Core/Reflection/V2` |
| Graph view of a system | Projection document | graph substrate or domain-specific projection owner |
| Plot drawing | Artist + draw list + backend | `Slab/Graphics/Plot2D/V2` |
| Workspace layout/surfaces | Workbench shell primitives | `Slab/Graphics/Window/V2` |
| Lab-specific panel/control | Immediate-mode app panel/controller | `Studios/LabV2` |
| CLI command | Command dispatcher over composition root | `Studios/CLI` |

If a feature spans several concerns, split it along these boundaries instead of creating one large manager.

## 6. Orthogonality Contract

V2 code should preserve these separations:

- A recipe constructs a run; it does not own UI.
- A session owns runtime state access; it does not decide presentation.
- A task drives execution; it does not own monitor windows.
- A trigger decides timing; it does not decide output side effects.
- A listener handles a sample; it does not change subscription topology at runtime unless explicitly designed for that.
- A live topic publishes state/control data; it does not own solver policy.
- A monitor observes snapshots; it does not directly control session internals by default.
- A model semantic object describes authored meaning; it is not a plot node or ImGui widget.
- A graph projection visualizes or edits an explicit document; it does not imply execution unless bound to a runtime graph artifact.
- A reflection operation describes invocable capability; it does not define the domain model.
- A LabV2 panel captures user intent and displays state; reusable behavior belongs outside the panel.

## 7. Review Checklist

Use this checklist during V2 reviews and agent handoffs:

1. Which stratum owns the change?
2. Does the chosen architecture match the stratum table?
3. Did any reusable policy land in `Studios/LabV2` that should move to `Slab/`?
4. Did UI code start polling runtime internals where a live snapshot/topic would be cleaner?
5. Did numeric/runtime code gain any ImGui, windowing, or app-target dependency?
6. Did model semantics gain presentation or task-manager dependencies?
7. Did reflection metadata remain descriptive rather than becoming the domain owner?
8. Are graph side effects explicit, or is a projection silently mutating runtime state?
9. Are legacy APIs isolated behind an adapter/service/module boundary?
10. Is any new singleton/global justified and bounded?
11. Did the relevant `Docs/index-*.md` or `Docs/status-v2.md` need an update?
12. Is there a focused build/test/smoke validation path for the touched stratum?

## 8. Explicit Non-Goals

Do not treat these as default V2 architectures:

- ECS across the codebase
- strict MVC across LabV2
- full CQRS/event sourcing
- plugin architecture as a replacement for CMake/library structure
- graph execution as the default meaning of every graph document
- runtime monitor injection as the default monitor path
- global service lookup as the normal way to cross boundaries

These patterns may be introduced for a bounded subsystem later, but they require an explicit design note and a clear owning stratum.

## 9. Migration Rules

V2 migration is boundary-first:

1. define the V2 contract
2. wrap legacy behavior behind an adapter if needed
3. move one consumer onto the contract
4. validate the seam
5. only then widen adoption

Rules:

- keep legacy paths operational during migration
- prefer adapters over rewrites when stabilizing a seam
- keep bridge code visibly named as bridge/legacy/adaptation code
- do not let temporary bridge constraints define the long-term V2 vocabulary
- update docs when a temporary bridge becomes a durable contract

## 10. Known Gaps

Current architecture gaps to account for:

- `Studios/LabV2` still contains large coordinator classes and direct knowledge of many domains.
- LabV2 has not fully adopted the V2 composition/service boundary.
- Some V2 systems still reach legacy globals directly or indirectly.
- graph documents are mostly projections today; execution/routing semantics are not uniformly mature.
- monitor snapshot delivery is the preferred direction, but compatibility read-lease paths still exist.
- some domain services are header-heavy and may need extraction after behavior stabilizes.

Treat these as migration pressure points, not as patterns to copy.
