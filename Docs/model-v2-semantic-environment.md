# Model V2 Semantic Environment

## Purpose

Record the current Model-tab semantic layering baseline in `LabV2`.

Use this when changing:
- `BaseVocabulary` / ambient semantic context
- model symbol/operator resolution
- semantic origin reporting
- Model-tab inspector/catalog behavior
- model semantic graph visibility and ODE realization readiness surfaces

## Code Map

- Model types and preset catalog:
  - `Slab/Core/Model/V2/ModelTypesV2.h`
- Notation context / parser integration:
  - `Slab/Core/Model/V2/ModelNotationV2.h`
- Semantic inspection / authoring diagnostics:
  - `Slab/Core/Model/V2/ModelAuthoringV2.h`
- ODE realization contract/extraction:
  - `Slab/Core/Model/V2/ModelRealizationV2.h`
- Seeded examples:
  - `Slab/Core/Model/V2/ModelSeedsV2.h`
- LabV2 Model tab:
  - `Studios/LabV2/LabV2WindowManager.h`
  - `Studios/LabV2/LabV2WindowManager.cpp`
  - `Studios/LabV2/LabV2WindowManagerModelPanels.cpp`
- Plot-based semantic graph:
  - `Slab/Graphics/Plot2D/V2/Artists/ModelSemanticGraphArtistV2.h`
- Coverage:
  - `Lib/tests/test_model_v2.cpp`
  - `Lib/tests/test_plot2d_v2.cpp`

## Current Layering

Treat the Model as having these conceptual parts:
- `BaseVocabulary`
- `Definitions`
- `Relations`
- `InitialConditions`
- `AssumedSemantics`
- `NotationContext`

Important:
- `BaseVocabulary` is a readonly ambient semantic environment.
- `Definitions` are explicit local model commitments.
- `Relations` are explicit local assertions/equations.
- `InitialConditions` are explicit model-level realization inputs for the current ODE slice.
- `AssumedSemantics` are inferred proposals/overlays.
- `NotationContext` is authoring/projection context, not a peer ontological layer beside `Definitions` and `Relations`.

Current realization-facing rule:
- ODE readiness is derived from canonical semantic overview data plus explicit model-level initial conditions.
- It is a projection from model semantics, not a separate authored runtime model.

## BaseVocabulary Contract

`BaseVocabulary` is semantic-first, not symbol-first.

Each entry carries ambient meaning plus one lightweight notation projection:
- semantic identity (`EntryId`, kind, semantic summary, source preset)
- optional definition-like role (`DefinitionKind`, declared type, operator style)
- canonical notation (`PreferredNotation` / symbol)
- optional preview notation (`PreviewLatex`) for convention-like entries

Examples:
- `\\mathbb{R}` is modeled as a scalar-set concept, not merely a token string
- `\\Box` is an ambient operator concept
- `\\dot` and `\\ddot` are notation conventions and should preview as templates rather than naked commands

## Resolution Order

Current precedence is:
1. local explicit `Definitions`
2. active `BaseVocabulary`
3. assumptions / inference

Rules:
- vocabulary entries do not silently materialize into local definitions
- local definitions may refine or override inherited vocabulary meaning
- semantic reports should expose where meaning came from

Current semantic origins:
- `LocalDefinition`
- `BaseVocabulary`
- inferred / assumed overlays for unresolved local roles

## Preset Baseline

### `core_math`

Ambient entries include:
- `\\mathbb{R}`
- `\\mathbb{C}`
- `\\mathbb{N}`
- `\\in`
- `\\to`
- equality / multiplication / power / grouping conventions

### `classical_mechanics`

Builds on `core_math` and adds:
- time-derivative notation conventions
- `\\dot{\\mathrm{state}}`
- `\\ddot{\\mathrm{state}}`

### `relativistic_field_theory`

Builds on `core_math` and adds:
- `\\Box`
- `\\partial_{\\mu}` placeholder
- spacetime-oriented notation placeholder

Non-goal in this layer:
- no forced metric signature, coordinates, discretization, solver, or full operator expansion

## UI Contract

Current Model workspace layout:
- separate dockable windows preserve semantic order:
  1. `Model Vocabulary`
  2. `Model Definitions`
  3. `Model Relations`
  4. `Model Notation Editor`
  5. `Model Assumptions`
  6. `Model Inspector`
- `Base Vocabulary` remains ambient/readonly
- `Definitions` and `Relations` remain the local editable content
- `InitialConditions` are currently visible through the summary/realization surfaces, not a dedicated editor pane yet
- `Assumptions` remain visible as inferred/proposed semantics, not silently canonized
- selection, cross-highlighting, and inspector provenance are shared across the docked surfaces

Summary and graph surfaces:
- the visibility toggle labeled `Summary` opens the panel titled `Model Layer`
- `Model Layer` shows semantic health/classification, the `Semantic Graph -> Open in Plots` action, and the current ODE realization summary
- `Model Semantic Graph` is rendered on `Plot V2` as a read-only knowledge-graph-like surface
- graph selection feeds back into the shared Model workspace selection state

Inspector/rendering rules:
- symbol-like entries render directly as LaTeX
- convention-like entries use preview LaTeX templates
- the UI should show whether a vocabulary entry is built-in, overridden locally, or specialized locally

Authoring rules:
- existing `Definitions` and `Relations` are edited transactionally in `Model Notation Editor`
- new `Definitions` and new `Relations` can be created directly from dedicated composers
- create/edit preview must stay transactional until `Apply` / `Create`
- unresolved-symbol errors still block canonical apply
- a missing symbol is resolved either by creating an explicit local `Definition` or by materializing an inferred assumption when that path exists

## Seed Expectations

### Harmonic Oscillator

- active preset: `classical_mechanics`
- ambient: `\\mathbb{R}`, derivative conventions
- local: `x`, `p`, `m`, `k`, `\\omega`
- explicit ODE initial conditions for `x` and `p`

### Damped Harmonic Oscillator

- active preset: `classical_mechanics`
- local: harmonic-oscillator baseline plus explicit `\\gamma`
- explicit first-order damping relation and ODE initial conditions

### Klein-Gordon

- active preset: `relativistic_field_theory`
- ambient: `\\mathbb{R}`, `\\Box`, minimal differential/operator language
- local: `\\phi`, `m`, Klein-Gordon relation

Important validation point:
- `\\Box` should resolve from ambient vocabulary rather than only from heuristic inference

## Known Gaps

- preset catalog is static and small
- no user-authored base-vocabulary editor yet
- indexed/tensor/operator language remains minimal
- override/specialization tracking is metadata-based, not full provenance/version history
- direct creation is single-object only; there is no batch authoring/undo stack yet
- draft-only objects are navigable in preview, but exact notation-span navigation is still absent
- there is no dedicated initial-condition editor surface yet
- there is no runtime bridge from the ODE descriptor into `FSimulationRecipeV2` yet
