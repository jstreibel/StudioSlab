# Plot V2 Node Graph Plan

**Status:** planning draft only (`2026-03-14`)  
**Audience:** AI agents and devs adding a node-graph surface under `Plots`

## Use This When

- planning a `Plots Graph` / plot-patchbay surface
- deciding whether a change belongs in Plot V2, graph substrate, or LabV2 shell
- selecting the first safe implementation slice for connectable artists

## Read After

1. `Docs/status-v2.md`
2. `Docs/index-labv2.md`
3. `Docs/graph-onthology.md`
4. `Docs/Plot2D-V2-Scheme.md`
5. `Docs/plot2d-v2-migration-plan.md`

## Goal

Add a runtime-first node graph in `Plots` where:

- live Plot V2 windows appear as nodes
- attached artists appear as child/runtime nodes
- right-click can add supported artists
- artist config stays reflection-driven
- typed artist wiring is added incrementally, not universally

## Current Leverage

- Graph substrate already has nodes, ports, edges, and canvas state.
- Shared LabV2 graph canvas already supports pan, drag, right-click add, and drag-to-connect.
- Plot reflection catalog already enumerates live windows and artists.
- `FRtoRFunctionArtistV2` already has a typed reflected function input path.

## Missing Contracts

- graph mode choice:
  - add `Visualization` / `PlotsGraph`
  - or reuse `Runtime` / `Routing` for the first slice
- artist factory registry for right-click creation
- first-class connectable artist port schema:
  - direction
  - carrier type id
  - semantic space id
  - binding kind (`ConfigValue`, `Handle`, `Stream`)
- live binding/update policy for edges
- stable handle/codec story for heavy inputs such as `FModelSemanticOverviewV2`
- persistence boundary:
  - live-only graph overlay
  - or graph-authored plot composition

## Recommended Slice Order

### `PG-00` Live Plot Graph Surface

- add a `Plots Graph` panel in `Plots`
- project live plot windows and artists into a graph document
- keep selection/focus synchronized with the plot host
- allow right-click `Add Artist` from a fixed registry
- use existing reflection inspector for artist/window config
- do not add artist-to-artist typed edges yet

### `PG-01` First Typed Connection Slice

- support one explicit path:
  - `Function -> FRtoRFunctionArtistV2`
- optional same-slice follow-up:
  - domain input for `FRtoRFunctionArtistV2`
- validate by type id and semantic space
- implement connections through reflected operations, not ad hoc casts

### `PG-02` General Artist Patchbay

- promote artist inputs/outputs to first-class graph ports
- add artist output surfaces only where they are semantically clear
- add a stable semantic-overview handle before making `FModelSemanticGraphArtistV2` connectable
- decide whether graph state is persisted/restored

## First-Slice Non-Goals

- replacing direct plot-host controls
- supporting arbitrary artist-to-artist coercions
- making every artist connectable on day one
- making plot graphs the authoritative authoring source

## Current Bias

- runtime-first, not compiler-first
- registry-driven artist creation
- explicit allowlist of connectable artist types
- one clean path first, generalization later

## Code Anchors

- `Slab/Core/Reflection/V2/GraphSubstrateV2.h`
- `Studios/LabV2/LabV2SubstrateGraphCanvas.cpp`
- `Slab/Graphics/Plot2D/V2/PlotReflectionCatalogV2.cpp`
- `Slab/Graphics/Plot2D/V2/Plot2DWindowV2.cpp`
- `Slab/Graphics/Plot2D/V2/Artists/RtoRFunctionArtistV2.cpp`
- `Slab/Graphics/Plot2D/V2/Artists/ModelSemanticGraphArtistV2.cpp`

## Stop Conditions

Stop and resolve contracts before implementation if:

- graph mode choice remains ambiguous
- output-port semantics are still fuzzy
- heavy handle ownership/lifetime is unresolved
- persistence is being mixed into the first runtime slice
