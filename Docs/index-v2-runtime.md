# Index: V2 Runtime, LiveData, LiveControl

## Use This When

- Adding or modifying V2 simulation slices
- Changing recipe/session/task behavior
- Wiring live data or live control topics/bindings
- Adding monitored execution paths shared by CLI and LabV2
- Designing or integrating reflection-driven interface/parameter/operation surfaces

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `SPEC.md` (V2 architecture and constraints)
3. When resuming an in-progress slice:
  - `Docs/index-handoffs.md`
  - `Docs/handoff-ode-realization-rz03.md`
4. One focused spec:
  - model semantic graph layering / canonical boundary: `Docs/index-model-semantic-graph.md`
  - ODE model->realization descent: `Docs/ode-realization-descent-plan.md`
  - control/input: `Docs/live-control-v2-spec.md`
  - live runtime params: `Docs/live-parameters-v2-slice-scope.md`
  - sequencing/time: `Docs/sequence-control-spec.md`
  - monitoring data-path tradeoffs: `Docs/monitoring-liveview-vs-listeners.md`
  - reflection contract/migration: `Docs/reflection-v2-contract.md`, `Docs/reflection-v2-migration-plan.md`, `Docs/reflection-v2-implementation.md`
  - plot reflection bridge quick path: `Docs/plot2d-reflection-v2-quick.md`
5. One active planning doc:
   - `Docs/v2-feature-backlog.md` or `Docs/v2-model-coverage-matrix.md`

## Current ODE Realization Boundary

Implemented already:
- ODE readiness/descriptor extraction from canonical `Model V2` semantics
- explicit model-level initial conditions for the ODE slice
- LabV2 visibility for readiness, selected relations, and initial state
- descriptor-driven runtime bridge from the ODE descriptor into `FSimulationRecipeV2` + `FStepperSessionV2`
- explicit numeric scalar bindings for parameters/initial symbols at runtime-build time

Still missing:
- model-owned numeric scalar binding authoring/configuration
- LabV2 launch path from an ODE-ready model into the runtime bridge

Use `Docs/handoff-ode-realization-rz03.md` for the post-`RZ-03` follow-up.

## Monitor Data Contract (Current)

- Default monitor data flow is push-to-snapshot via `SessionLiveViewV2`:
  - task emits live-view events at monitor interval
  - `SessionLiveViewV2` publishes telemetry/status and latest copied snapshot (consumer-gated)
  - passive monitor reads latest snapshot + version and redraws on version change
- Keep listener subscriptions for deterministic analytics/transforms and persisted snapshot pipelines.
- Runtime monitor open/close should not require changing task subscription topology.

Quick start for new monitored slices:
1. ensure the recipe is built with a `SessionLiveViewV2`
2. monitor window registers/unregisters snapshot consumption on `SessionLiveViewV2`
3. monitor reads via `TryGetSnapshot()` and redraws only when version changes
4. add listener subscriptions only when deterministic side-effects/analytics are required

## Core Code Map

- Runtime core:
  - `Slab/Math/Numerics/V2/Runtime/SimulationRecipeV2.h`
  - `Slab/Math/Numerics/V2/Runtime/SimulationSessionV2.h`
  - `Slab/Math/Numerics/V2/Runtime/StepperSessionV2.h`
  - `Slab/Math/Numerics/V2/Task/NumericTaskV2.h`
- Model-to-runtime boundary:
  - `Slab/Core/Model/V2/ModelTypesV2.h`
  - `Slab/Core/Model/V2/ModelRealizationV2.h`
  - `Slab/Core/Model/V2/ModelRealizationRuntimeV2.h`
  - `Slab/Core/Model/V2/ModelSeedsV2.h`
- Live topics/hubs:
  - `Slab/Math/Data/V2/LiveDataHubV2.h`
  - `Slab/Math/Data/V2/LiveControlHubV2.h`
  - `Slab/Math/Data/V2/SessionLiveViewV2.h`
- Legacy interface seed (adapter source for reflection migration):
  - `Lib/Core/Controller/Interface.h`
  - `Lib/Core/Controller/Parameter/Parameter.h`
- Reflection V2 adapter/invoke path:
  - `Slab/Core/Reflection/V2/ReflectionTypesV2.h`
  - `Slab/Core/Reflection/V2/ReflectionInvokeV2.h`
  - `Slab/Core/Reflection/V2/LegacyInterfaceAdapterV2.h`
- Shared simulation slices:
  - `Slab/Studios/Common/Simulations/V2/*.h`
  - `Slab/Studios/Common/Simulations/V2/*.cpp`
- Shared monitors/listeners:
  - `Slab/Studios/Common/Monitors/V2/*.h`
  - `Slab/Studios/Common/Monitors/V2/*.cpp`
- Shared task/run glue:
  - `Slab/Studios/Common/NumericsV2TaskUtils.h`
  - `Slab/Studios/Common/V2SimulationRunners.h`

## Integration Entry Points

- CLI dispatch:
  - `Studios/CLI/main.cpp`
- LabV2 dispatch:
  - `Studios/LabV2/SimulationManagerV2.h`
  - `Studios/LabV2/SimulationManagerV2.cpp`
- LabV2 schemes/panel host (reflection surface insertion point):
  - `Studios/LabV2/LabV2WindowManager.h`
  - `Studios/LabV2/LabV2WindowManager.cpp`
- LabV2 model realization visibility:
  - `Studios/LabV2/LabV2WindowManagerModelPanels.cpp`

## Validation Minimum

- Build:
  - `cmake --build cmake-build-debug --target testsuite -j8`
- Tests:
  - `ctest --test-dir cmake-build-debug --output-on-failure`
- Graphics/runtime smoke when relevant:
  - `cmake --build cmake-build-debug --target SlabTests StudioSlab -j8`
  - `./Build/bin/SlabTests list`
  - `./Build/bin/StudioSlab`
  - `./Build/bin/testsuite "[ModelV2]"`
