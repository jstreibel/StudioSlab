# Handoff: ODE Realization `RZ-03`

## Use This When

- You are continuing the ODE model-to-runtime descent
- You need to know what is already implemented before touching runtime code
- You want the narrowest safe `RZ-03` landing zone

## Current Boundary

Already implemented:
- `RZ-00`: ODE realization contract in `Slab/Core/Model/V2/ModelRealizationV2.h`
- `RZ-01`: readiness and descriptor extraction from canonical `Model V2` semantics
- `RZ-02`: explicit model-level initial conditions in `Slab/Core/Model/V2/ModelTypesV2.h`
- LabV2 summary surface for ODE readiness and initial-state visibility
- plot-based semantic graph for model navigation in `LabV2`

Validated examples:
- harmonic oscillator: positive case
- damped harmonic oscillator: positive case
- Klein-Gordon: negative boundary check for the ODE-first descent

## `RZ-03` Goal

Build one narrow runtime bridge from `FODERealizationDescriptorV2` into the existing V2 runtime path.

Target shape:
- descriptor in
- simple recipe/session/stepper path out
- diagnostics first
- oscillator-family scope only

## Runtime Landing Zone

Current V2 runtime seam:
- `Slab/Math/Numerics/V2/Runtime/SimulationRecipeV2.h`
- `Slab/Math/Numerics/V2/Runtime/SimulationSessionV2.h`
- `Slab/Math/Numerics/V2/Runtime/StepperSessionV2.h`
- `Slab/Math/Numerics/V2/Task/NumericTaskV2.h`

Existing ODE stepping substrate:
- `Lib/Math/Numerics/ODE/Solver/LinearStepSolver.h`
- `Lib/Math/Numerics/ODE/Steppers/Euler.h`
- `Lib/Math/Numerics/ODE/Steppers/RungeKutta4.h`
- `Lib/Math/Numerics/ODE/Solver/EquationState.h`

Reference recipe pattern:
- `Lib/Models/Stochastic-Path-Integral/V2/SPI-RecipeV2.cpp`

## Recommended First Slice

1. Keep the bridge descriptor-driven. Do not re-infer state/parameter roles in runtime code.
2. Define one deterministic state ordering from `descriptor.StateVariables`.
3. Build one small scalar/vector `EquationState` implementation for that ordered state.
4. Build one narrow `LinearStepSolver` adapter that evaluates the selected first-order relations.
5. Wrap it in `FStepperSessionV2` through one simple `FSimulationRecipeV2`.

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

## Key Missing Piece To Respect

There is still no general `Model V2` runtime expression evaluator.

That means the first bridge should stay narrow:
- either implement a tiny scalar arithmetic evaluator for the selected descriptor expressions
- or keep the first bridge explicitly limited to oscillator-family arithmetic

In either case, do not hide the limitation.

## Suggested Validation

- unit tests for deterministic state ordering and initial-state mapping
- runtime build test for harmonic oscillator
- runtime build test for damped harmonic oscillator
- explicit blocked-path test for Klein-Gordon

## Likely UI Follow-Up

After the runtime bridge exists, add one minimal LabV2 launch path from an ODE-ready model.

Keep that launch path secondary to the runtime contract itself.
