# Model Semantic Graph Layering

## Purpose

Define the intended layering for Model semantic graphs so ontology can grow cleanly while overlays, provenance, draft state, and debug/export surfaces remain available.

Use this when:
- changing Model semantic graph projection/export
- deciding whether graph data is canonical or overlay
- extending Model ontology for new math/physics
- deciding what Realization may consume
- reviewing graph JSON or plot surfaces

## Read Order

1. `Docs/status-v2.md`
2. `Docs/model-v2-semantic-environment.md`
3. this file
4. `Docs/graph-onthology.md`
5. `Docs/ode-realization-descent-plan.md`

## Working Decision

Separate three layers:
- canonical ontology graph
- overlay graph data
- projections

Short rule:
- open-world authoring
- closed-world realization

## 1. Canonical Ontology Graph

What belongs here:
- semantically meaningful model objects that should remain stable across views, exports, and runtime handoff
- typed relations whose meaning is part of the model rather than part of authoring/debugging
- ambient concepts only when they are semantically active in the model

Current minimal seed:
- `Space`
- `Domain`
- `Coordinate`
- `Field`
- `StateVariable`
- `Parameter`
- `Operator`
- `Equation`
- `Observable`
- `InitialCondition`

Current minimal edge seed:
- `acts_on`
- `maps_between`
- `depends_on`
- `constrains`
- `valued_in`
- `defined_over`
- `has_coordinate`
- `initializes`
- `observes`

Invariants:
- every canonical node and edge must have explicit semantic meaning
- canonical identity must be stable across plot, JSON, and realization projections
- hop limits, layout, selection, and rendering choices are not part of the canonical graph
- the canonical graph should be assumption-closed before realization consumes it

## 2. Overlay Graph Data

Overlays are useful and expected, but they are not the canonical semantic topology.

Typical overlay classes:
- `Assumption`
- provenance links
- source/target links
- draft deltas
- conflict markers
- validation issues
- materialization hints
- view-only/navigation helpers

Rules:
- overlays may attach to canonical nodes or edges
- overlays may be shown in plots and exports
- overlays must be labeled as non-canonical
- overlays must not silently become realization inputs

## 3. Projections

A projection is a consumer-facing view of the graph:
- Plot V2 semantic graph
- JSON export
- inspector summaries
- realization extractors

Rules:
- projections may be neighborhood-limited or task-specific
- a projection may mix canonical and overlay data if labels stay explicit
- a projection is not the contract

## 4. Near-Term Mapping From Current Model V2 Objects

Current exported kinds map roughly as:
- `Definition` -> canonical typed object once classified (`Field`, `Parameter`, `Coordinate`, ...)
- `Relation` -> canonical `Equation` or `Constraint`
- `VocabularyEntry` -> canonical ambient concept when semantically active
- `Assumption` -> overlay only
- `SourceLink` / `TargetLink` -> overlay provenance only

This means the current semantic-graph plot remains useful, but it should be interpreted as an overlay-heavy projection of model semantics rather than as the final ontology.

## 5. Realization Boundary

Realization should consume only the canonical, typed, assumption-closed subgraph.

Realization should ignore:
- provenance-only edges
- view-only edges
- draft-only overlays
- pending assumptions that have not been canonized

If a realization path depends on overlay-only meaning, the model is not ready for descent.

## 6. Extension Rule

When adding support for a new model or mathematical representation:
1. map it into existing canonical node and edge kinds if possible
2. add overlays for provisional or debug structure
3. extend the canonical ontology only when the new meaning cannot be expressed cleanly
4. add consistency checks at the canonical layer, not only in view code

Goal:
- extending the system should feel like mapping more meaning into one coherent ontology, not inventing a new graph each time
