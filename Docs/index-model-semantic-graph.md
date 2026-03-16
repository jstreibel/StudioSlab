# Index: Model Semantic Graph

## Use This When

- changing the Model semantic graph plot or export
- deciding canonical versus overlay graph semantics
- extending model ontology for new math or physics
- connecting Model semantics to Realization
- reviewing graph JSON structure

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `Docs/model-v2-semantic-environment.md`
3. `Docs/model-semantic-graph-layering.md`
4. `Docs/model-semantic-levels.md`
5. one boundary doc:
   - `Docs/graph-onthology.md`
   - `Docs/ode-realization-descent-plan.md`
6. if touching LabV2 UI:
   - `Docs/index-labv2.md`
7. if touching runtime/realization:
   - `Docs/index-v2-runtime.md`

## Core Rules

- treat the plotted/exported graph as a projection, not as the contract
- keep canonical ontology separate from overlays, provenance, and debug structure
- keep `kind`, `carrier/level`, and `representation` distinct when reviewing semantic mismatches
- realization consumes only the canonical typed subgraph
- keep context small: read one ontology doc and one realization doc, not the full docs tree

## Core Docs

- `Docs/model-v2-semantic-environment.md`
- `Docs/model-semantic-graph-layering.md`
- `Docs/model-semantic-levels.md`
- `Docs/graph-onthology.md`
- `Docs/ode-realization-descent-plan.md`

## Relevant Code Map

- `Slab/Core/Model/V2/ModelAuthoringV2.h`
- `Slab/Core/Model/V2/ModelTypesV2.h`
- `Slab/Core/Model/V2/ModelRealizationV2.h`
- `Studios/LabV2/LabV2WindowManager.cpp`
- `Studios/LabV2/LabV2WindowManagerModelPanels.cpp`
- `Slab/Graphics/Plot2D/V2/Artists/ModelSemanticGraphArtistV2.h`
- `Lib/tests/test_model_v2.cpp`
- `Lib/tests/test_plot2d_v2.cpp`
