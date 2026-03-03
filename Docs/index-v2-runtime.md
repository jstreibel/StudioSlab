# Index: V2 Runtime, LiveData, LiveControl

## Use This When

- Adding or modifying V2 simulation slices
- Changing recipe/session/task behavior
- Wiring live data or live control topics/bindings
- Adding monitored execution paths shared by CLI and LabV2

## Read Order (Smallest Useful Set)

1. `Docs/status-v2.md`
2. `SPEC.md` (V2 architecture and constraints)
3. One focused spec:
   - control/input: `Docs/live-control-v2-spec.md`
   - live runtime params: `Docs/live-parameters-v2-slice-scope.md`
   - sequencing/time: `Docs/sequence-control-spec.md`
   - monitoring data-path tradeoffs: `Docs/monitoring-liveview-vs-listeners.md`
4. One active planning doc:
   - `Docs/v2-feature-backlog.md` or `Docs/v2-model-coverage-matrix.md`

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
  - `Slab/Math/Numerics/V2/Task/NumericTaskV2.h`
- Live topics/hubs:
  - `Slab/Math/Data/V2/LiveDataHubV2.h`
  - `Slab/Math/Data/V2/LiveControlHubV2.h`
  - `Slab/Math/Data/V2/SessionLiveViewV2.h`
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

## Validation Minimum

- Build:
  - `cmake --build cmake-build-debug --target testsuite -j8`
- Tests:
  - `ctest --test-dir cmake-build-debug --output-on-failure`
- Graphics/runtime smoke when relevant:
  - `cmake --build cmake-build-debug --target SlabTests StudioSlabV2 -j8`
  - `./Build/bin/SlabTests list`
  - `./Build/bin/StudioSlabV2`
