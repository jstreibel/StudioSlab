# Functional Minimization V2 Slice Scope (Draft)

## Goal

Define a low-entropy first V2 migration slice for functional minimization workflows (currently represented by `Particle-Trajectory-Action`).

## Legacy Anchor

- Studio entrypoint: `Studios/FunctionalMinimization/FM-Main.cpp`
- Underlying numerics path: `Math/Numerics/Metropolis/RtoR/RtoR-Action-Metropolis-Recipe.h`
- Current UX pattern: immediate viewer hookup + ad-hoc setup before run.

## First V2 Slice (`FM-00`)

- `V2-H`: finite-step functional minimization run via `FSimulationRecipeV2` + `FNumericTaskV2`.
- `V2-M`: passive monitor showing candidate trajectory + telemetry.
- Reuse existing R->R plotting and session live-view pipeline from current V2 slices.

## Minimal Runtime Contract

- Execution config:
  - `N` (path discretization points)
  - boundary values (`q0`, `qf`)
  - domain interval (`t0`, `tf`)
  - `steps`, `temperature`, `proposal-scale`
  - `interval`, `monitor-interval`, `batch`
- Published state:
  - current candidate path (`R->R` numeric field)
  - scalar diagnostics: action estimate, acceptance ratio, boundary residual

## Monitor Surface (`FM-01`)

- Left pane:
  - run telemetry (step/progress/reason)
  - minimization diagnostics (action, acceptance, residual)
- Right pane:
  - current candidate trajectory plot
  - optional reference/ground-truth overlay when available

## Out of Scope (for first slice)

- Multi-trajectory population methods
- Adaptive proposal schedules
- Non-Euclidean / constrained path spaces
- Sequence integration and authored presentation tracks

## Acceptance Criteria

1. `Studios` CLI exposes `fmin`/`functional-min` V2 subprogram.
2. Headless run is reproducible for fixed seed/config.
3. Passive monitor can observe a live run without interfering with progression.
4. At least one V2 test validates run completion and diagnostic bounds.

## Suggested Implementation Order

1. Add `Lib/Models/.../V2` recipe + state for trajectory minimization.
2. Add `Slab/Studios/Common/Simulations/V2/FunctionalMinimizationSliceV2`.
3. Add passive monitor window (`R->R` plot + stats).
4. Wire CLI + LabV2 launcher.
5. Add `testsuite` coverage and one `SlabTests` monitor smoke command.
