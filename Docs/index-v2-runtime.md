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
4. One active planning doc:
   - `Docs/v2-feature-backlog.md` or `Docs/v2-model-coverage-matrix.md`

## Monitor Data Contract (StudioSlabV2 direction)

- Monitor data flow should be push-based:
  - trigger -> snapshot listener -> passive monitor (or topic bridge when needed)
- Delivery policy for monitor snapshots should be `LatestOnly`.
- New monitor work should avoid direct session polling/read-lease loops in UI paths.

Quick start for new monitored slices:
1. define a compact copied snapshot payload for the view
2. attach a snapshot listener at the simulation trigger point
3. expose latest snapshot through listener handle (or stable topic when externalized)
4. render monitor windows from the latest received snapshot only

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
