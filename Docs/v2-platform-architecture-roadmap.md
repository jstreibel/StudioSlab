# V2 Platform Architecture Roadmap

## Purpose

This document defines the target architecture direction for StudioSlab V2:

```text
StudioSlab V2 = platform kernel + capability contracts + backend implementations + extension modules + app composition roots
```

Use this when planning architectural work that crosses several V2 domains.

This is a roadmap and direction document.
For boundary rules, use `Docs/v2-architecture-charter.md`.
For implementation status, use `Docs/status-v2.md`.

## Executive Direction

StudioSlab V2 should evolve into a capability-oriented modular platform.

The platform core should be small, explicit, and boring:

- runtime context
- platform lifecycle
- module loading/registration
- typed services
- capability/backend selection
- reflection discovery/governance
- diagnostics/configuration

Most behavior should arrive as extensions:

- numerics module
- live data/control module
- reflection module
- Plot2D module
- model-family modules
- ontology module
- graph modules
- workbench modules
- backend modules
- temporary legacy bridge modules

The goal is not to create a dynamic plugin system immediately.
The near-term goal is a modular monolith with explicit composition and low entropy.

## 1. Target Architecture Stack

```text
Apps / Composition Roots
  LabV2, CLI, WebGL-WASM, standalone Studios

Extension Modules
  Numerics, LiveData, Reflection, Plot2D, ModelV2, Ontology, Graph, model families,
  workbench panels, CLI commands, backend installers, legacy bridges

Capability Services
  Task execution, reflection catalog, live data/control, plotting, rendering,
  asset storage, GUI, graph runtime, model catalog, artifact store

Platform Hosts
  Headless, GLFW, SFML, SDL, Browser

Backends
  OpenGL, WebGL, CPU, CUDA, filesystem, ImGui, recording

Platform Kernel
  RuntimeContext, service registry, module registry, platform host, diagnostics,
  configuration/profile, capability descriptors

Foundational Code / Legacy Adapters
  Lib models/kernels, bounded legacy bridges, third-party dependencies
```

## 2. Core Concepts

| Concept | Meaning | Owns | Does Not Own |
| --- | --- | --- | --- |
| Platform Kernel | Small shared runtime foundation | context, registries, lifecycle hooks, diagnostics, config | domain behavior, UI panels, solver loops |
| Platform Host | Process/window/event-loop host | startup/shutdown, frame loop, event pump, host policy | rendering semantics, module behavior |
| Capability Contract | Typed interface for one capability family | interface vocabulary and invariants | implementation choice |
| Service | Runtime object implementing a capability | behavior, lifecycle, runtime state | unrelated registration |
| Backend | Swappable implementation of one capability | concrete implementation details | app composition |
| Module | Registration/contribution bundle | service factories, backends, reflection sources, panels, commands, defaults | event loop, hidden global state |
| Composition Root | App-local assembly point | module set, platform host, selected backends, app policy | reusable platform behavior |
| Reflection Registry | Discovery/governance plane for commands and queries | interface catalogs, parameter/operation metadata, invocation policy | domain ownership, service lifetime |
| Legacy Bridge | Temporary anti-corruption layer | adapts legacy APIs to V2 contracts | new V2 vocabulary |

## 3. Kernel Scope

The platform kernel should contain only cross-cutting foundation.

Kernel-owned:

- `FRuntimeContextV2`
- typed service registry
- module descriptors and module registry
- runtime profile/config fragments
- platform host selection hooks
- capability/backend descriptors
- diagnostics and startup validation
- reflection registry access point or integration seam

Kernel-forbidden:

- ImGui panels
- numerical stepping code
- model semantics
- concrete OpenGL/WebGL calls
- concrete solver kernels
- LabV2 app state
- direct legacy task/window globals except through bridge services

Rule:

- if it cannot be described without naming a domain feature, it probably does not belong in the kernel.

## 4. Modules vs Services

The distinction should stay strict:

```text
Module registers or contributes.
Service performs runtime work.
Backend implements one service capability.
Composition root chooses which modules/backends are active.
```

Examples:

| Need | Module | Service / Backend |
| --- | --- | --- |
| run numeric tasks | `NumericsModuleV2` | `ITaskServiceV2`, task executor backend |
| expose reflected interfaces | `ReflectionModuleV2` | `IReflectionServiceV2`, reflection registry |
| draw plots | `Plot2DModuleV2` | `IPlotRenderBackendV2`, plot registry |
| use OpenGL | `OpenGLRenderingModuleV2` | OpenGL render backend service |
| launch XY simulations | `XYModelModuleV2` | recipe factory, model catalog contribution |
| show Lab panels | `LabWorkbenchModuleV2` | workbench panel registry contribution |

Modules should be mostly declarative registration units.
They should not become large managers.

## 5. Module Taxonomy

### 5.1 Core Platform Modules

Purpose:
- install foundation services that most apps need
- keep kernel minimal while making common services opt-in

Examples:
- `ReflectionModuleV2`
- `DiagnosticsModuleV2`
- `LiveDataModuleV2`
- `TaskModuleV2`

### 5.2 Capability Modules

Purpose:
- define or install a capability family

Examples:
- `NumericsModuleV2`
- `Plot2DModuleV2`
- `GraphModuleV2`
- `AssetStoreModuleV2`
- `ArtifactModuleV2`

### 5.3 Backend Modules

Purpose:
- register a concrete implementation behind a capability contract

Examples:
- `OpenGLRenderBackendModuleV2`
- `WebGLRenderBackendModuleV2`
- `ImGuiGuiBackendModuleV2`
- `CpuTaskBackendModuleV2`
- `CudaTaskBackendModuleV2`
- `FilesystemAssetBackendModuleV2`

### 5.4 Domain / Model Modules

Purpose:
- contribute scientific model families, recipes, controls, artifacts, and reflection surfaces

Examples:
- `IsingModelModuleV2`
- `XYModelModuleV2`
- `KGModelModuleV2`
- `MolecularDynamicsModuleV2`
- `SPIModuleV2`

These modules may contribute:
- recipe factories
- model semantic seeds
- default run presets
- runtime controls
- reflection sources
- artifact schemas
- optional monitor/panel registrations

### 5.5 Workbench Modules

Purpose:
- contribute UI surfaces to Lab-like apps without making LabV2 the owner of reusable behavior

Examples:
- `LabSimulationLauncherModuleV2`
- `LabModelWorkspaceModuleV2`
- `LabPlotInspectorModuleV2`
- `LabGraphPlaygroundModuleV2`

Workbench modules may register panels, menus, commands, and workspace surfaces.
They should delegate actual domain behavior to services.

### 5.6 Bridge Modules

Purpose:
- adapt legacy behavior while V2 grows

Examples:
- `LegacyBridgeModuleV2`
- `LegacyReflectionBridgeModuleV2`
- `LegacyTaskBridgeModuleV2`
- `LegacyWindowBridgeModuleV2`

Bridge modules are temporary.
They should be easy to identify and easy to delete.

## 6. Capability Map

| Capability | Current Direction | Target Shape |
| --- | --- | --- |
| Platform hosting | V2 platform host seed, legacy backend wrapper | host selected by app composition root |
| Service resolution | typed service registry seed | typed services with capability descriptors and diagnostics |
| Reflection | V2 catalog/operation model, adapters | global discoverable command/query governance plane |
| Task execution | legacy task manager bridge plus numeric task V2 | task service with backend/executor options |
| Numerics | recipe/session/task pipeline | module-registered recipe/session/listener ecosystem |
| Live data/control | topics/hubs and session live view | shared services, topic registries, transport-neutral DTOs |
| Plotting | Plot2D artists/draw lists/backend seam | Plot2D module plus backend modules and artist registry |
| Rendering | mostly legacy/OpenGL-specific paths | rendering capability contracts and backend modules |
| GUI | LabV2/ImGui direct usage plus shared shell pieces | GUI/workbench services, panels as module contributions |
| Graphs | projections and playground usage | separated semantic projections, node graphs, and runtime graphs |
| Models | DDD-inspired Model V2 plus model recipes | model catalog and model-family modules |
| Artifacts | first ODE time-series artifacts | artifact store/catalog service and provenance schemas |
| Legacy | direct globals plus first services | bounded bridge modules, then removal |

## 7. Reflection As Governance Plane

Reflection should be the discoverability and governance layer for runtime capabilities.

Reflection should answer:

- what interfaces exist
- which source/module owns an interface
- which parameters are observable or mutable
- which operations are queries or commands
- whether an operation is safe while running
- which thread affinity applies
- which UI/CLI/graph affordances can be generated
- which capabilities are experimental, deprecated, or bridge-backed

Reflection should not:

- own domain state
- replace the service registry
- become a UI framework
- become the graph runtime
- own persistence

Target direction:

- modules register reflection sources
- services expose reflected interfaces where appropriate
- apps use reflection to discover and govern capabilities
- reflection metadata includes ownership, stability, runtime policy, and capability tags

## 8. Graph Architecture Direction

Graphs should not be treated as one subsystem.

Separate graph classes:

| Graph Class | Meaning | Default Behavior |
| --- | --- | --- |
| Semantic graph | view of model/ontology/reflection meaning | projection/read-only or controlled authoring |
| Node graph | user-authored computational or routing document | editable document |
| Runtime graph | executable instantiated graph | explicit runtime artifact |
| Scheme graph | reusable recipe/control/sequence graph | compiled or lowered before execution |
| Routing graph | connections between capabilities/topics/controls | explicit connect/disconnect operations |

Rule:

- a graph projection must not silently imply execution.
- execution begins only when a graph is lowered into an explicit runtime artifact or command.

## 9. Numerical Pipeline Direction

The current recipe/session/task architecture should remain the execution core.

Target module split:

| Layer | Responsibility |
| --- | --- |
| Numerics core | recipe/session/task contracts, scheduler, listener contracts |
| Task service | submit/list/cancel/observe tasks |
| Executor backend | CPU thread, CUDA, remote, browser-local, recording |
| Model module | recipe factories, presets, controls, model-specific reflection |
| Live module | snapshot/status/telemetry/control topics |
| Artifact module | deterministic capture, provenance, export |
| Workbench module | launch panels, monitors, task views |

Rule:

- model modules may provide recipes and controls.
- numerics core owns the pipeline abstractions.
- executor backends own execution mechanics.
- workbench modules own presentation and launch UX.

## 10. Comparison With Current Docs And Specs

### `SPEC.md`

Alignment:
- already defines V2 as boundary-level refactor, not rewrite
- already separates `Slab`, `Lab`, and `Studios`
- already names Numerics V2, LiveData V2, monitors, and shared slices

Roadmap adds:
- a stronger platform-kernel model
- a module/service/backend distinction
- reflection as governance plane
- a long-term legacy-removal direction

Potential update later:
- add the one-line target architecture formula to `SPEC.md` once this direction is proven by one or two implementation slices.

### `Docs/repo-architecture-overview.md`

Alignment:
- current canonical split remains valid
- `Slab/` remains the shared V2 landing zone
- `Studios/` remains app/composition-root territory

Roadmap adds:
- a more explicit modular-platform interpretation of `Slab/`
- a stronger rule that `LabV2` is an app shell, not the platform center

### `Docs/backends-platforms-modules-v2-plan.md`

Alignment:
- this roadmap directly adopts its vocabulary: `PlatformHost`, `Backend`, `Module`, `Service`, `Connector`, `Composition Root`
- no conflict

Roadmap adds:
- module taxonomy
- target stack
- staged migration plan
- comparison against current systems

### `Docs/v2-architecture-charter.md`

Alignment:
- charter defines the rules and review checklist
- this roadmap defines the migration destination and sequence

Relationship:
- charter is the law
- roadmap is the path

### `Docs/index-v2-runtime.md`

Alignment:
- current runtime pipeline remains the target execution core
- monitor snapshot direction remains preferred

Roadmap adds:
- numerics should become a capability/module family over time
- model recipes, task services, live topics, artifacts, and workbench launchers should become separable contributions

### `Docs/index-labv2.md`

Alignment:
- LabV2 remains the discovery workbench
- existing shell/window extraction remains the right direction

Roadmap adds:
- LabV2 should progressively become a composition root and module consumer
- reusable panels/controllers should become workbench module contributions when stable

### Reflection Docs

Alignment:
- current Reflection V2 already uses command/query operation metadata
- adapter-first migration remains correct

Roadmap adds:
- reflection should become globally discoverable and governable
- reflection should carry module/source ownership and capability metadata
- reflection should not replace services or modules

### Model / Semantic Graph Docs

Alignment:
- model semantics stay domain-owned
- graph views are projections unless explicitly promoted

Roadmap adds:
- separate semantic graphs, node graphs, runtime graphs, scheme graphs, and routing graphs
- model families can become modules that contribute semantics, recipes, controls, and artifacts

### `Docs/status-v2.md`

Current status:
- several foundation pieces already exist
- full modular platform architecture is not yet implemented

Interpretation:
- current code is a seed/bridge state
- this roadmap should guide future slices without claiming they are complete today

## 11. Current Foundation Assessment

Already present:

- V2 runtime context/service registry seed
- platform host vocabulary and legacy host bridge
- first legacy service bridge
- Numerics V2 recipe/session/task pipeline
- LiveData/LiveControl topic infrastructure
- reflection operation/catalog model
- Plot2D backend seam and artist architecture
- shared workspace shell/layout extraction
- model semantic/realization pipeline
- LabV2 as active discovery workbench
- CLI as first partial composition-root adoption

Main entropy sources:

- LabV2 still owns too much coordination and state
- reflection registry exists, but module ownership/governance is not fully formalized
- modules are still shallow compared to the desired extension model
- backend/platform/rendering concepts are partially separated but not uniformly applied
- legacy globals still leak into some V2 flows
- graph semantics are still exploratory and mixed
- artifacts/provenance are early

## 12. Roadmap

### Phase 0: Lock Vocabulary And Rules

Status:
- active / documentation-level

Goals:
- use `PlatformHost`, `Backend`, `Module`, `Service`, `Composition Root` consistently
- keep `Docs/v2-architecture-charter.md` as the rule document
- use this roadmap for planning cross-domain architecture work

Exit criteria:
- new V2 design docs cite the charter and this roadmap when crossing platform/module boundaries

### Phase 1: Strengthen The Kernel Contract

Goals:
- clarify module descriptors, dependency metadata, provided capabilities, and required capabilities
- improve service registration diagnostics
- define startup phases such as register, configure, validate, start, stop
- keep the kernel independent from graphics, numerics, and LabV2 specifics

Likely slices:
- module descriptor expansion
- capability descriptor vocabulary
- runtime profile validation
- service collision and missing-dependency diagnostics

### Phase 2: Make Composition Roots Explicit

Goals:
- keep `Studios/CLI` as the first bounded composition-root proving ground
- migrate LabV2 toward explicit runtime profile/module selection
- prevent hidden startup through legacy globals

Likely slices:
- LabV2 runtime-context construction path
- shared profile builders for headless, desktop, and browser-safe apps
- app-local module selection lists

### Phase 3: Convert Existing V2 Systems Into Modules

Goals:
- wrap existing systems as registration modules without rewriting their internals
- preserve current behavior
- make dependencies explicit

Candidate modules:
- `ReflectionModuleV2`
- `LiveDataModuleV2`
- `NumericsModuleV2`
- `Plot2DModuleV2`
- `ModelV2Module`
- `OntologyModuleV2`
- `LegacyBridgeModuleV2`

Exit criteria:
- apps can install these modules through a common module path
- module installation produces services/registries that current code can still consume

### Phase 4: Separate Backend Modules From Platform Hosts

Goals:
- make host selection independent from render/backend selection
- reduce confusion inherited from legacy `Backend`
- allow one app to use multiple capability backends

Likely slices:
- OpenGL plot/render backend module
- ImGui GUI backend module
- headless host module/profile
- GLFW host module/profile
- recording/null render backend for tests

Exit criteria:
- rendering backends no longer imply ownership of the app event loop

### Phase 5: Modularize Model Families

Goals:
- make model families first-class contributors to the platform
- avoid hardcoding model launch/catalog knowledge in LabV2

Candidate modules:
- `IsingModelModuleV2`
- `XYModelModuleV2`
- `KGModelModuleV2`
- `SPIModuleV2`
- `MolecularDynamicsModuleV2`

Each module may contribute:
- model semantic seeds
- recipe factories
- presets
- runtime controls
- reflection sources
- artifact schemas
- optional monitor/workbench panel contributions

### Phase 6: Define Workbench Extension Points

Goals:
- let LabV2 consume panel/menu/workspace contributions from modules
- keep LabV2 as app shell and discovery surface
- move stable reusable panel behavior out of the monolithic window manager

Likely extension points:
- workspace registrations
- panel surface registrations
- command palette actions
- launcher cards
- plot inspectors
- graph node libraries
- artifact viewers

Exit criteria:
- a model-family module can add a launcher/panel without editing the central LabV2 manager for every detail

### Phase 7: Clarify Graph Families

Goals:
- separate semantic graph projections from node graphs and runtime graphs
- keep side effects explicit
- define lowering boundaries for executable graphs

Likely slices:
- graph document type metadata
- graph ownership/source metadata
- explicit runtime graph artifact type
- routing graph command/reflection integration

Exit criteria:
- every graph surface can answer whether it is projection, authoring document, or executable runtime artifact

### Phase 8: Artifact And Provenance Platform

Goals:
- promote early ODE time-series capture into a shared artifact platform
- make runs, datasets, snapshots, and provenance discoverable

Likely slices:
- artifact catalog service
- artifact schema registration
- run provenance metadata
- export/import connector hooks
- Lab artifact viewer extension point

### Phase 9: Legacy Retirement

Goals:
- remove legacy bridge modules after V2 equivalents are proven
- prevent new features from depending on legacy globals

Likely slices:
- replace direct task-manager access
- replace legacy backend startup paths
- retire legacy reflection adapters per source
- migrate stable model recipes to V2-native services/catalogs

Exit criteria:
- legacy bridge modules can be disabled for selected app profiles

### Phase 10: Optional External Plugin Boundary

This should come last.

Do not introduce dynamic loading or external plugin packaging until the modular monolith boundary is stable.

Prerequisites:
- stable module descriptor
- stable service/capability descriptors
- stable reflection governance metadata
- stable resource/artifact ownership model
- validation story for third-party modules

## 13. Near-Term Planning Priorities

Recommended next architecture slices:

1. define module lifecycle and capability metadata
2. move LabV2 startup toward explicit runtime-context/module composition
3. wrap existing Reflection/LiveData/Numerics/Plot2D systems as modules
4. define workbench extension registries before extracting more LabV2 panels
5. create one model-family module as the vertical proof, likely XY or Ising
6. add source/module ownership metadata to reflection catalogs
7. define graph document class metadata before expanding graph execution

## 14. Design Guardrails

Avoid:

- making every object a module
- letting modules do runtime work directly
- letting reflection replace services
- letting LabV2 remain the only integration point
- treating graph visualization as graph execution
- adding dynamic plugin loading before static modules are disciplined
- moving legacy concepts into V2 vocabulary unchanged

Prefer:

- static modules first
- typed services over string lookups
- backend selection per capability
- explicit composition roots
- reflection metadata for governance
- snapshot/dataflow observation over UI polling
- small bridge modules that can be deleted

## 15. One-Line North Star

StudioSlab V2 should become a capability-oriented modular scientific workbench platform, where apps compose a small kernel, explicit modules, typed services, and selected backends; LabV2 is one workbench built on top, not the architectural center.
