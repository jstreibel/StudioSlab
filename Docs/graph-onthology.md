# StudioSlab Graph Ontology and Semantic Layer

**Status:** Design direction with partial implementation baseline (`2026-03-18`)  
**Audience:** StudioSlab core devs (reflection/runtime/graph tooling)  
**Motivation:** Build a coherent graph system that supports:
- instantiable, reusable simulation setups (Template graphs),
- live runtime introspection/control (Runtime graphs),
- patchbay-style binding between existing endpoints (Routing graphs),
- reusable visualization pipelines (Viz graphs),
- a mathematical “meaning layer” (Semantic/Theory graph) with LaTeX.

## 0. Implementation Snapshot (`2026-03-06`)

Implemented substrate baseline:
- shared graph model in `Slab/Core/Reflection/V2/GraphSubstrateV2.h`
- graph modes currently represented in substrate enum:
  - `Semantic`, `Template`, `Runtime`, `Routing`, `SchemesBlueprint`
- common carriers in code:
  - node/edge/port/member metadata and policy structs
  - graph document + canvas state
  - reflection-catalog-to-graph adapter projection

Current LabV2 usage:
- Schemes `Blueprint Graph` now builds node/edge/canvas view state from substrate documents.
- Graph Playground `Semantic`, `Template`, `Runtime`, and `Routing` modes use substrate documents as source-of-truth.
- Graph Playground mode canvases share substrate canvas behavior (pan/grid state) and a common substrate-based renderer path.

Current ontology-inspection baseline:
- split-file ontology viewer is implemented under `Resources/Ontologies`
- loader/projection path lives in `Slab/Core/Ontology/V2/OntologyGraphV2.h`
- LabV2 exposes a dedicated read-only `Ontology` workspace with:
  - `Ontology Overview`
  - `Ontology Focus`
  - `Ontology Layer`
  - `Ontology Inspector`
- current projection merges one selected study with the persistent global ontology
- current layout is deterministic and read-only; it is not yet an ontology editor

Not yet implemented from this ontology:
- semantic operator execution compilation (`Template -> Runtime`)
- first-class visualization graph mode
- semantics-driven coercion/rewrite passes

---

## 1. Core Thesis

StudioSlab should not force one universal “graph” to mean everything. Instead, we define **multiple graph modes** with crisp semantics, all built on a **shared substrate**:

- **Template Graph**: declarative, instantiable “recipe” graphs.
- **Runtime Graph**: stateful graphs representing what’s running *now*.
- **Routing Graph**: patchbay-like wiring between existing endpoints/resources.
- **Visualization Graph**: visualization pipelines reusable by type or bound to instances.
- **Semantic/Theory Graph**: category-like objects & morphisms (spaces/operators/relations).

This prevents long-term ontology debt while still offering a unified user experience.

### 1.1 Current Semantic Spine

Keep the end-to-end ontology story this small:

1. `Model` owns authored canonical mathematical meaning.
   - This is the semantic source for classification, readiness, and descent.
2. `Realization` consumes only canonical semantic structure.
   - Descent is a projection from model meaning, not a second semantic model.
3. `Resources/Ontologies` holds the persistent global/study ontology layer.
   - It carries semantic classes, requirements, solver/recipe/artifact relations, and study-local activation state.
4. The `Ontology` workspace is the read-only navigator over that persistent semantic layer.
   - `Overview` is the compact map.
   - `Focus` is the readable neighborhood navigator.
5. Runtime sessions, datasets, and numerical artefacts are instance/projection surfaces, not canonical ontology.
   - They may realize or satisfy ontology structure, but they are not the semantic source of truth.

---

## 2. Key Ontological Separations (Non-negotiable)

These separations keep the system coherent:

### 2.1 Template vs Instance
- **Template**: an archetype/recipe; can be instantiated multiple times. (“Platonic”)
- **Instance**: an existing runtime entity with identity. (“Mundane”)

**Implication:** Nodes and edges must know whether they refer to *templates* or *instances*.

### 2.2 Schema/Type vs Value vs Handle
- **Schema/Type**: shape of data (“bits-level” type); used to validate and encode/decode.
- **Value**: owned data payload (copy semantics).
- **Handle/Resource**: reference to an existing entity/dataset/stream/viewer/etc. (identity semantics).

**Implication:** An edge must never be ambiguous: value flow ≠ handle binding ≠ stream subscription.

### 2.3 Property vs Operation vs Event
- **Property**: state slot (get/set); has mutability and exposure policies.
- **Operation**: callable behavior; has query/command semantics and execution policies.
- **Event/Stream**: reactive signals or data streams; push semantics; subscription semantics.

**Implication:** A graph UI can render them similarly (ports), but execution semantics differ.

### 2.4 Implementation Operation vs Semantic Operator
- **Implementation operation**: a concrete callable (reflected method/handler).
- **Semantic operator**: mathematical meaning (morphism), optionally with LaTeX and declared laws.

**Implication:** Template graphs should prefer semantic operators; runtime graphs show bound implementations.

---

## 3. Existing Foundation (Reflection V2)

StudioSlab already has strong core primitives:

- **Values:** `FReflectionValueV2 { TypeId, Encoded }` and `FValueMapV2`.
- **State schema:** `FParameterSchemaV2` (id/name/type + semantics like mutability/exposure).
- **Action schema:** `FOperationSchemaV2` with typed I/O fields and policy metadata.
- **Entity boundary:** `FInterfaceSchemaV2` groups parameters + operations (+ tags/children).
- **Central enforcement:** `InvokeOperationV2` enforces query/command and runtime policies.

This is an excellent base for graph tooling, and should remain the “policy enforcement spine.”

---

## 4. The Common Graph Substrate (Shared Across Modes)

All graph modes should share the same underlying vocabulary:

### 4.1 Carrier Types (bits-level)
- `TypeId` / `CTypeId` describes the encoded representation used by `FReflectionValueV2`.
- This is required for serialization and safe invocation.

### 4.2 Semantic Spaces (meaning-level)
Introduce `FSpace` as “semantic type” that wraps a carrier type + meaning:

- `CarrierTypeId`: the bits-level type.
- `Tags`: e.g., `field`, `spectrum`, `mesh`, `scalar`, `function`.
- `Attrs`: domain, units, basis, sampling, boundary conditions, etc.
- Optional: compatibility rules / equivalence keys.

**Goal:** Avoid “Field2D<Real> is just a 2D array.” It’s a field over a domain with units, BCs, etc.

### 4.3 Ports
A **port** is a typed connection point.

Minimal port declaration:
- name, direction (in/out)
- carrier type id
- optional semantic space id
- multiplicity (single, list, stream)
- optional policy hints (e.g., streaming vs snapshot)

### 4.4 Handles/Resources
A handle represents identity-bound references:
- dataset handle
- run handle
- viewer handle
- external device endpoint

A handle must include:
- identity (stable id)
- capability interface id
- optional semantic space(s) of the resource outputs

### 4.5 Interfaces & Members
A (reflected) interface exposes a capability surface. Conceptually:

- **Properties** (state slots; maps to `FParameterSchemaV2`)
- **Operations** (callable actions; maps to `FOperationSchemaV2`)
- **Events/Streams** (optional future extension)
- **Factories** (constructors/spawners; may be modeled as operations or explicit member kind)

### 4.6 Policies
Reflection V2 already defines key policies:
- query/command kind
- thread affinity
- run-state policy (restart required, staged apply, etc.)
- side-effect class

**Invariant:** The execution engine must use centralized enforcement (like `InvokeOperationV2`)
for any operation invoked via graphs.

### 4.7 Semantic Operators and Bindings
Introduce:

- `FSemanticOperator`: abstract operator meaning (morphism) with:
    - domain/codomain spaces (ports),
    - LaTeX symbol/template,
    - declared properties (linear, invertible, etc.),
    - relations (inverse/adjoint/equivalent).

- `FImplementationBinding`: mapping from a semantic operator to one or more concrete reflected operations/factories, with constraints.

**Result:** One semantic operator (e.g., Fourier) can have multiple implementations (FFTW, GPU, fallback).

---

## 5. Graph Modes (Semantics of Nodes and Edges)

### 5.1 Template Graph (Blueprint / Recipe)
**What it represents:** a reusable, instantiable pipeline/spec.

- Nodes: semantic operators; template factories; constants; graph parameters.
- Edges: value flow (typed by carrier type and semantic space).
- No hard references to specific runtime instances unless explicitly bound.

**Primary use-cases:**
- simulation setups parameterized by inputs
- reusable analysis pipelines
- reproducible runs

**Execution model:**
- Compiled/instantiated into runtime graph(s), selecting implementations based on bindings, constraints, device, etc.

---

### 5.2 Runtime Graph (Instance Execution Graph)
**What it represents:** what is running *now* (identity + state + time).

- Nodes: instance interfaces (running simulation, viewer instance, dataset instance).
- Edges: runtime flows (streams, subscriptions) and control channels; may include value snapshots.
- All nodes/edges are instance-bound (identity matters).

**Primary use-cases:**
- monitoring, telemetry, debugging
- interactive control (pause/step/restart/apply staged changes)
- resource lifetime visibility (what exists, who depends on what)

---

### 5.3 Routing Graph (Patchbay / Binding Graph)
**What it represents:** live wiring between existing endpoints/resources.

- Nodes: endpoints/ports on existing instances/devices/viewers.
- Edges: connections (connect/disconnect) with immediate side effects.

**Primary use-cases:**
- attach an existing viewer to a specific run output
- pipe stream A into sink B
- rewire audio/data endpoints live (Helvum-like behavior)

**Invariant:** routing edges operate on handles; they are imperative.

---

### 5.4 Visualization Graph (Viz Pipeline)
**What it represents:** a visualization pipeline reusable by type or bound to instances.

Two valid approaches:
1) **As a Template Graph specialization** (recommended early): viz nodes are semantic operators of kind `Visualization`.
2) **Separate mode** if viz has distinct resource contexts (GPU pipeline, render graph semantics, etc.).

**Primary use-cases:**
- “for any Field2D, show heatmap+contours”
- “attach this viz pipeline to Run#17.temperatureField”
- reusable viewer templates

---

### 5.5 Semantic/Theory Graph (Spaces + Operators + Relations)
**What it represents:** mathematical meaning and admissible transformations.

- Nodes: spaces and operators (objects and morphisms).
- Edges: operator arrows, equivalences, coercions, inverse/adjoint relations, inclusions.

**Primary use-cases:**
- browse/search operators by domain/codomain and properties
- validation and auto-suggestion (“you connected Field → SpectrumView; suggest Fourier”)
- render graph pipelines as LaTeX expressions
- make domain knowledge explicit and reusable

Near-term `Model V2` bridge:
- the current Model semantic graph should distinguish:
  - canonical ontology
  - overlays/provenance/debug structure
  - projections such as plots, JSON exports, and realization extractors
- this lets the user-facing graph carry useful non-ontological information without polluting the canonical semantic topology
- see `Docs/model-semantic-graph-layering.md`

**Note:** Not all theory edges must be executable. Executability depends on bindings.

---

## 6. Mode Interactions (Platonic → Mundane Pipeline)

The intended flow:

1) **Semantic/Theory** defines spaces/operators and relations.
2) **Template Graphs** compose semantic operators into reusable recipes.
3) **Compilation/Instantiation** produces:
    - runtime instances,
    - bound concrete implementations,
    - resources/handles,
    - scheduling/dependency plan.
4) **Runtime Graph** reflects executing system state.
5) **Routing Graph** binds runtime endpoints to other endpoints (viewers/sinks/controllers).
6) **Viz Graph** templates bind to types (general) or handles (specific instances).

---

## 7. Invariants and Rules of Thumb

### 7.1 Edge Meaning Must Be Explicit
Every edge must declare one of:
- Value flow (copy semantics)
- Handle binding (identity semantics)
- Stream subscription (push semantics)
- Control dependency (execution semantics, if used)

Do not overload “wire” to mean all at once.

### 7.2 Template Graphs Prefer Semantic Operators
Template graphs should avoid direct calls to reflected implementation operations unless:
- the operation is itself semantic and stable, or
- it is wrapped by a semantic operator with a binding.

### 7.3 Centralized Policy Enforcement
All operation invocations triggered by graphs go through policy enforcement (thread affinity, run-state rules, side-effects).

### 7.4 Coercions Must Be Nodes (Not Silent)
If data representation changes (resampling, unit conversion, real→complex embedding), represent it as an explicit coercion operator node unless trivial and lossless.

### 7.5 “Parameter” is Overloaded — Use Precise Terms
Prefer:
- **Property** (state slot)
- **Argument** (operation input)
- **Graph Parameter** (template input)

### 7.6 Semantics ≠ Proof
Semantic properties (linear, invertible, etc.) are **declared**, not proven.
They are used for:
- search
- UX hints
- validation warnings
- optional optimization passes

---

## 8. Minimal Semantic Seed Library (Recommended First Set)

Spaces (examples):
- ScalarReal, ScalarInt, Bool
- VectorReal (or ArrayReal)
- Field2DReal (domain-tagged)
- Spectrum1DComplex (or placeholder if complex not yet present)
- FunctionRtoR (already exists in tests)
- VizHandle (opaque)

Operators (examples):
- Identity
- FourierTransform / InverseFourierTransform
- Laplacian
- Gradient / Divergence
- Average
- Energy / Norm
- Resample (coercion/projection)
- PlotField / PlotSpectrum (visualization kind)

Bindings:
- Map at least 2 semantic operators to real reflected operations to validate integration early.

---

## 9. Example Scenarios (How It Should Feel)

### 9.1 Reusable simulation setup
User builds a Template Graph:
- `HeatEquationTemplate` factory
- connect graph parameters (dt, domain size, diffusion constant)
- connect `RunOutput(Field2DReal)` → `PlotField`

Instantiates multiple runs with different parameters → creates multiple runtime instances.

### 9.2 Attach viewer to an existing run (patchbay)
Routing Graph:
- pick `Run#17.temperatureField` output handle
- connect to `Viewer#3.inputField`
  Immediate effect: viewer attaches to the live output.

### 9.3 Typed suggestions via semantics
User connects a `Field2DReal` output into a `SpectrumView` input.
System suggests:
- “Insert FourierTransform (Field2DReal → Spectrum1DComplex)”
  because semantic registry knows a morphism exists with compatible spaces.

### 9.4 LaTeX view
A Template Graph pipeline can render:
- node chain as a composed expression (e.g., `E(u) = ∫ |∇u|^2 dx`)
  where operators carry LaTeX templates and spaces carry symbols.

---

## 10. Implementation Roadmap (Pragmatic)

### Phase 1: Semantic Core + Overlay (no UI rewrite) [active]
- Implement `FSpace`, `FSemanticOperator`, `FImplementationBinding` registry.
- Implement a **semantics overlay** keyed by (interfaceId,paramId) and (interfaceId,opId,portName).
- Seed a small operator/space library.
- Bind a couple of semantic ops to real reflection ops.
- Add unit tests.

### Phase 2: Typed pin graph substrate (new execution graph) [partial]
- Introduce a true typed-pin execution graph representation (dataflow).
- Allow Template Graph compilation to Runtime Graph.
- Use semantic matching + coercion nodes.

### Phase 3: Routing graph formalization [partial]
- Standardize handle-based endpoints.
- Add connect/disconnect operations + policy enforcement.
- Provide patchbay view.

### Phase 4: Visualization templates + binding [todo]
- First-class viz templates as Template Graphs or dedicated mode.
- “Attach viewer template to handle” workflows.

### Phase 5: Semantic relations and rewrites (optional optimization) [todo]
- inverse/adjoint links
- basic rewrite hints (e.g., `F^{-1}∘F = Id`)
- suggestion and simplification passes

---

## 11. Migration Strategy (From Current State)

Current state:
- Reflection V2 is solid and widely used.
- A shared graph substrate exists and is integrated in LabV2 Schemes + Graph Playground surfaces.
- LabV2 graph UI is still primarily reflection-driven/schema-driven; substrate is the common data model, not yet a full execution compiler.
- Legacy “Blueprints” exists with hardcoded PinType.

Migration approach:
1) **Keep Reflection V2 intact** as the enforcement and capability description layer.
2) Add semantics as an **overlay + registry**, not as a refactor of V2.
3) Gradually adopt semantic operators for template graphs and typed execution graphs.
4) Optionally bridge old Blueprints by mapping PinType → FSpace/FType or by generating semantic operator nodes.

Key benefit:
- No forced rewrite of LabV2; semantics can be queried and displayed incrementally.
- New typed-pin graphs can be introduced without breaking existing reflection-driven UI.

---

## 12. Glossary (Project Terms)

- **Carrier Type**: bits-level type id used for encoding/decoding values.
- **Space**: semantic type (meaning), carrying tags/attrs, used for compatibility.
- **Semantic Operator**: abstract mathematical operation (morphism), with LaTeX and relations.
- **Binding**: mapping from semantic operator to concrete reflected operation(s).
- **Template Graph**: instantiable recipe graph.
- **Runtime Graph**: live instance graph.
- **Routing Graph**: patchbay/binding graph for existing endpoints.
- **Viz Graph**: visualization pipeline (template or bound).
- **Interface**: reflected capability surface grouping properties/operations (and possibly events/factories).

---

## 13. Design Principle (Why This Works)

This architecture makes “class vs instance” explicit:
- Template graphs and semantic operators are the “Platonic forms.”
- Runtime graphs and routing graphs are the “particulars.”
- The bridge is explicit and typed: spaces/operators bind to implementations; instances bind by handles.

It keeps StudioSlab:
- scientifically expressive (meaning-first),
- technically safe (policy enforcement),
- and practically usable (live routing and visualization).
