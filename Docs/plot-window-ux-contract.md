# Plot Window UX Contract (V2 Direction)

## Scope

This document defines the minimal UX contract for `FPlot2DWindow` controls.

Goal:
- keep existing plot controls/data
- remove detached/floating `Plot Detail` behavior
- stay on a linear implementation path (no large windowing refactor)

## Contract

1. Plot controls are viewport-attached.
- use an in-viewport toolbar anchored to the plot corner.
- avoid standalone floating detail windows as default behavior.

2. Detail panel is plot-scoped, not workspace-scoped.
- it opens as an attached inspector near the same viewport.
- it is not docked into global workspace docking trees.

3. Existing capabilities must remain available.
- auto adjust toggle
- fit/review range
- save graph
- artist visibility
- artist z-order controls
- artist-specific GUI controls

## UX Mapping

Legacy `Plot Detail` content maps to:
- `Toolbar`: `Fit`, `Auto`, `Show/Hide UI`, `Save`, overflow menu (`...`)
- `Detail > View`: range numbers + global plot actions
- `Detail > Layers`: visibility and z-order per artist
- `Detail > Artist Controls`: per-artist GUI blocks

## Definition Of Done

`done` means:
- `Plot Detail` no longer spawns as detached tiny floating window.
- controls render attached to the plot viewport in a stable position.
- all existing `Plot Detail` controls are still reachable from new UI.
- right-click popup actions still work.
- build/tests remain green.

## Out Of Scope

- replacing ImGui
- global docking/layout architecture rewrite
- plot rendering backend rewrite
