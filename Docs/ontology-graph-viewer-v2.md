# Ontology Graph Viewer V2

## 0. Status

- First-pass implementation is in place as of `2026-03-18`.
- Scope is intentionally narrow:
  - load split ontology JSON files
  - validate softly
  - build a combined read-only graph projection
  - inspect it inside the dedicated `Ontology` workspace

## 1. Canonical Boundary

### 1.1 JSON Files

- `studioslab-ontology.schema.json` defines the contract and enums.
- `global-descent.ontology.json` defines the persistent, model-independent descent map.
- `*.study.json` defines local authored mathematical content plus study-local activation/satisfaction/blockage state over the global ontology.

### 1.2 Non-negotiable Interpretation

- JSON is canonical.
- The graph is projection-only.
- The viewer does not mutate ontology state.
- Stable ids must survive end to end.

## 2. Internal Runtime Model

### 2.1 Document Types

- `FOntologySchemaDocument`
- `FGlobalDescentOntologyDocument`
- `FStudyOntologyDocument`
- `FOntologyGraphBundle`
- `FOntologyGraphProjection`

### 2.2 Bundle Responsibilities

- hold schema metadata and enum sets
- load global nodes and edges
- load study-local nodes and edges
- build node/edge tables keyed by stable ids
- build incoming/outgoing adjacency indexes
- preserve ownership and source-document provenance
- accumulate soft diagnostics

### 2.3 Projection Responsibilities

- merge global ontology with one selected study
- classify nodes by layer and kind
- classify edges by type
- derive activation/satisfaction/blockage overlays
- derive render summaries and filter visibility
- keep global structure visually distinct from study-local authored content

## 3. Load and Validation Flow

### 3.1 Current Load Path

1. discover ontology resources under `Resources/Ontologies`
2. load schema
3. load global ontology
4. discover and load study documents
5. build `FOntologyGraphBundle`
6. build one selected-study `FOntologyGraphProjection`

### 3.2 Validation Contract

Current soft checks include:

- missing required node fields
- missing required edge fields
- duplicate ids within a document
- broken edge references
- unknown node kinds / edge kinds / layers
- unknown activation status values
- malformed study references to global ids

Diagnostics must not crash rendering. Unknown extra fields are tolerated and preserved where practical.

## 4. LabV2 Surface

### 4.1 Workspace

- dedicated top-level tab: `Ontology`

### 4.2 Panels

- `Ontology Overview`
  - full selected-study ontology map
  - compact glyph-style node rendering
- `Ontology Focus`
  - neighborhood projection derived from the shared selection
  - readable card rendering for navigation/detail
  - falls back to the selected study root when nothing is selected
  - defaults to a first-hop neighborhood
- `Ontology Layer`
  - study selection + filters + status
- `Ontology Inspector`
  - stable id
  - source file
  - ownership scope
  - kind/type
  - layer
  - abstract flag
  - raw properties payload
  - incoming/outgoing connections

## 5. Rendering Semantics

### 5.1 Visual Distinctions

- layers are color-distinguished
- global ontology is quieter/structural
- study-local authored content is more prominent
- activation status is overlaid explicitly
- blocked state is intentionally high-contrast

### 5.2 Current Interaction

- click node/edge to select
- overview and focus plots share the same selection
- hover summary cards
- incident highlight
- inspector synchronization
- focus view follows the current selection as a neighborhood navigator
- when nothing is selected, focus anchors on the selected study root
- focus depth stays intentionally small by default and widens explicitly through the layer controls
- host-level pan/zoom/fit controls

## 6. Layout and Text Scaling

### 6.1 Layout

- deterministic layered layout
- global ontology stays in stable layer columns
- selected-study local nodes occupy sidecar space near the owning study root
- non-selected studies' local nodes are excluded from the selected-study projection entirely
- LabV2 requests fit-to-graph passes when the selected study, filters, or focus neighborhood rebuild
- edge routing is simple and deterministic

### 6.2 Text and Node Size

- node sizing uses font metrics plus estimated rendered string width
- plot-space ontology text now scales with graph zoom
- node rectangles and badge reservations use the same zoom-derived scale so text and boxes stay coherent
- overview mode now uses one-line compact labels and no in-node category/footer text
- focus mode keeps the richer card treatment and friendly category labels (`semantic`, `study`, `requirement`, `solver`, etc.)
- study and requirement nodes use distinct silhouettes on top of category color so the category remains visible at a glance

### 6.3 Current Writer Constraint

- the current path uses the existing `FWriter` / `FWriterOpenGL` transform stack
- `FTextCommandV2::FontScale` is now honored by the Plot V2 OpenGL backend
- this is the narrow first implementation
- if higher zoom ranges require sharper text, the next extension should use freetype-gl SDF render modes instead of pushing bitmap glyph scaling further

### 6.4 Writer-State Invariant

- Plot V2 ontology text is emitted in screen-space after plot-to-viewport conversion.
- The shared theme writer can also be used by other backends that install a pen-position transform.
- Before drawing ontology text, the Plot V2 OpenGL backend must reset that pen-position transform to identity.
- Without that reset, node boxes and labels can diverge under zoom because geometry and glyphs are no longer evaluated in the same coordinate space.

## 7. Tests and Example Coverage

### 7.1 Resource Coverage

- `global-descent.ontology.json`
- `harmonic-oscillator.study.json`
- `klein-gordon.study.json`

### 7.2 Current Tests

- bundle load and validation path
- deterministic projection path
- neighborhood projection path
- dense-column spacing baseline
- harmonic oscillator reachable ODE -> RK4 -> artifact path
- Klein-Gordon blocked requirements and realization path filters
- overview-vs-focus render-density behavior
- artist render + click selection
- ontology text zoom scaling behavior

Main test file:

- `Lib/tests/test_ontology_graph_v2.cpp`

## 8. Non-goals For This Slice

- ontology editing
- JSON write-back
- force-directed layout
- ontology mutation from the graph view
- multi-study comparative reasoning
- solver execution directly from the ontology graph
