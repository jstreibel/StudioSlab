# Handoff: ODE Realization `RZ-03`

## Status Note (`2026-03-16`)

- The core `RZ-03` runtime bridge is implemented.
- The first follow-up integration slice is also implemented:
  - minimal lab-local numeric-binding authoring in the Model workspace
  - one headless LabV2 launch path from oscillator-family ODE-ready models into the runtime bridge
- Remaining work is now beyond that first launch/config slice.

## Use This When

- You are continuing the post-`RZ-03` ODE runtime follow-up work
- You need to know what is already implemented before touching runtime code
- You want the narrowest safe next step after the core runtime bridge landed

## Current Boundary

Already implemented:
- `RZ-00`: ODE realization contract in `Slab/Core/Model/V2/ModelRealizationV2.h`
- `RZ-01`: readiness and descriptor extraction from canonical `Model V2` semantics
- `RZ-02`: explicit model-level initial conditions in `Slab/Core/Model/V2/ModelTypesV2.h`
- `RZ-03`: descriptor-driven runtime bridge in `Slab/Core/Model/V2/ModelRealizationRuntimeV2.h`
- LabV2 summary surface for ODE readiness and initial-state visibility
- LabV2 Model workspace numeric-binding config surface for required scalar symbols
- LabV2 headless `Run` path for oscillator-family ODE-ready models
- plot-based semantic graph for model navigation in `LabV2`

Validated examples:
- harmonic oscillator: positive case
- damped harmonic oscillator: positive case
- Klein-Gordon: negative boundary check for the ODE-first descent

## What Landed In `RZ-03`

- explicit first-order runtime-system builder from `FODERealizationDescriptorV2`
- narrow scalar AST evaluator for literal / symbol / unary / binary arithmetic
- runtime handoff into `FSimulationRecipeV2` + `FStepperSessionV2`
- oscillator-family validation on harmonic and damped oscillator
- explicit diagnostics for missing numeric bindings and unsupported expression kinds

## Current Follow-Up Goal

Keep the bridge narrow while moving beyond the first headless oscillator launch path.

## Runtime Landing Zone

Current V2 runtime seam:
- `Slab/Math/Numerics/V2/Runtime/SimulationRecipeV2.h`
- `Slab/Math/Numerics/V2/Runtime/SimulationSessionV2.h`
- `Slab/Math/Numerics/V2/Runtime/StepperSessionV2.h`
- `Slab/Math/Numerics/V2/Task/NumericTaskV2.h`
- `Slab/Core/Model/V2/ModelRealizationRuntimeV2.h`

Existing ODE stepping substrate:
- `Lib/Math/Numerics/ODE/Solver/LinearStepSolver.h`
- `Lib/Math/Numerics/ODE/Steppers/Euler.h`
- `Lib/Math/Numerics/ODE/Steppers/RungeKutta4.h`
- `Lib/Math/Numerics/ODE/Solver/EquationState.h`

Reference recipe pattern:
- `Lib/Models/Stochastic-Path-Integral/V2/SPI-RecipeV2.cpp`

## Recommended Follow-Up Slice

1. Keep the bridge descriptor-driven. Do not re-infer state/parameter roles in runtime code.
2. Keep numeric bindings lab-local until there is a clear model-owned numeric-parameter story.
3. Add monitoring / live-data only if there is a concrete visualization target for the model-driven runtime.
4. Broaden beyond oscillator-family models only after the current headless path is stable.

## Constraints

Do:
- require `descriptor.IsReady`
- require one initial assignment per realized state
- keep state ordering stable and testable
- fail with explicit diagnostics when a descriptor cannot be executed

Do not:
- broaden into PDE support
- introduce solver catalogs yet
- mix live controls into the first bridge
- build a second semantic-model inference layer in runtime code

## Key Remaining Gaps

- `Model V2` still does not own concrete numeric parameter values.
- The runtime bridge currently relies on an explicit scalar-binding map supplied at build time.
- The current LabV2 launch path is headless and oscillator-family only.
- There is no dedicated monitor/live-data visualization path for model-driven ODE runs yet.

## Suggested Validation

- keep `ModelV2` runtime tests for harmonic and damped oscillator passing
- keep the required-binding ordering stable and testable
- keep Klein-Gordon blocked before launch

## Likely UI Follow-Up

The next useful user-facing slice is:
- a clearer handoff from `Run (Headless)` into monitor/task inspection
- a monitored visualization path once there is a concrete state/plot presentation for model-driven ODE runs
