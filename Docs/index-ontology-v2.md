# Index: Ontology Graph Viewer V2

## Use This When

- working on the split-file ontology viewer under `Resources/Ontologies`
- changing ontology JSON loading, validation, projection, or rendering
- wiring the dedicated `Ontology` workspace in `LabV2`
- debugging global-vs-study-local graph semantics
- checking deterministic layout, filter behavior, or inspector provenance

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `Docs/ontology-graph-viewer-v2.md`
3. `Docs/graph-onthology.md`
4. if touching LabV2 shell/layout: `Docs/index-labv2.md`
5. if comparing against model-authored semantic graphs: `Docs/index-model-semantic-graph.md`
6. if validating changes: `Docs/index-testing.md`

## Core Rules

- JSON files under `Resources/Ontologies` are canonical.
- The graph surface is a read-only projection, never the source of truth.
- Keep schema, global ontology, and study-local state distinct end to end.
- Preserve stable ids from JSON through bundle, projection, selection, and inspector.
- Validation is soft:
  - emit diagnostics
  - keep rendering when possible
  - preserve unknown extra fields when feasible
- Layout must stay deterministic for the same inputs.

## Resource Contract

- Schema document:
  - `Resources/Ontologies/studioslab-ontology.schema.json`
- Global descent ontology:
  - `Resources/Ontologies/global-descent.ontology.json`
- Study documents:
  - `Resources/Ontologies/*.study.json`

## Core Code Map

- Bundle / validation / projection:
  - `Slab/Core/Ontology/V2/OntologyGraphV2.h`
- Plot V2 artist:
  - `Slab/Graphics/Plot2D/V2/Artists/OntologyGraphArtistV2.h`
  - `Slab/Graphics/Plot2D/V2/Artists/OntologyGraphArtistV2.cpp`
- Plot V2 backend hook for text scaling:
  - `Slab/Graphics/Plot2D/V2/Backends/OpenGLRenderBackendV2.cpp`
- LabV2 workspace wiring:
  - `Studios/LabV2/LabV2WindowManager.h`
  - `Studios/LabV2/LabV2WindowManager.cpp`
  - `Studios/LabV2/LabV2WindowManagerOntologyPanels.cpp`
- Validation/tests:
  - `Lib/tests/test_ontology_graph_v2.cpp`

## Current UI Surface

- Top-level workspace tab:
  - `Ontology`
- Default docked windows:
  - `Ontology Overview`
  - `Ontology Focus`
  - `Ontology Layer`
  - `Ontology Inspector`
- Current interactions:
  - click node/edge selection from either ontology plot
  - overview-to-focus synchronized navigation
  - hover summary
  - incident highlight
  - selected-study switching
  - scope/filter toggles
  - focus-hop control for the neighborhood surface
  - fit/zoom/pan through Plot V2 host controls

## Current Render Notes

- The ontology navigator now uses two explicit render modes:
  - overview: compact map glyphs
  - focus: readable neighborhood cards
- The fitted overview intentionally suppresses category/footer text inside nodes; color, silhouette, and corner tags carry most of the type/scope/status signal.
- The focus surface derives a filtered neighborhood projection from the shared selection instead of trying to make the full graph readable at once.
- Focus falls back to the selected study root when nothing is selected.
- Focus defaults to a first-hop neighborhood; the `Ontology Layer` panel can widen it.
- In LabV2, ontology projection rebuilds request host fit-to-graph passes for both overview and focus surfaces.
- Selected-study projection now excludes other studies' local nodes entirely instead of leaving them visible with unset positions.
- Plot V2 must reset any shared-writer pen transform before drawing ontology text, because the ontology backend already converts plot coordinates into screen-space pen positions.
- If future zoom ranges demand sharper large-scale text, the next step is an SDF-backed freetype-gl path rather than unbounded bitmap scaling.

## Validation Minimum

- Build:
  - `cmake --build cmake-build-debug --target StudioSlab testsuite -j8`
- Targeted tests:
  - `./Build/bin/testsuite "[OntologyGraphV2]"`
- Useful broader checks:
  - `./Build/bin/testsuite "[ModelV2]"`
  - `./Build/bin/StudioSlab`

## Known Gaps

- One selected study is projected at a time.
- Multi-study comparison is not implemented.
- Ontology editing / JSON write-back is not implemented.
- Render-time text zoom currently uses transformed glyph geometry, not an SDF-specific writer path.
