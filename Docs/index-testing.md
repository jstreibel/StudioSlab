# Index: Build, Tests, and Smoke Validation

## Use This When

- Preparing or validating any code change
- Running V2 runtime and LabV2 smoke checks
- Verifying graphics/ImGui migration behavior

## Standard Build/Test Commands

- Configure (example):
  - `cmake -S . -B cmake-build-debug -DSTUDIOSLAB_CUDA_SUPPORT=ON`
- Build library and key targets:
  - `cmake --build cmake-build-debug --target Slab StudioSlabV2 SlabTests testsuite -j8`
- Run tests:
  - `ctest --test-dir cmake-build-debug --output-on-failure`

## Runtime Smoke (V2-Focused)

- `./Build/bin/StudioSlabV2`
- `./Build/bin/SlabTests list`
- `./Build/bin/SlabTests spi-live-monitor-mock --seconds 5`
- `./Build/bin/SlabTests metropolis-monitor-smoke --seconds 5`
- `./Build/bin/SlabTests moldyn-monitor-smoke --seconds 5`
- `./Build/bin/SlabTests xy-monitor-smoke --seconds 5`
- `./Build/bin/SlabTests ising-monitor-smoke --seconds 5`

## ImGui/Blueprint Validation Helpers

- `Docs/validation/imgui-blueprints-smoke.md`
- `Docs/validation/imgui-upstream-parity-log.md`
- `Scripts/imgui-runtime-smoke.sh`
- `Scripts/imgui-provider-runtime-matrix.sh`
- `Scripts/imgui-upstream-runtime-spike.sh v1.91.0`

## Where Tests Live

- Catch2 testsuite entry:
  - `Studios/Tests/SlabTestsMain.cpp`
- Conventional unit tests:
  - `Lib/tests/`

## Change Review Checklist

1. Build succeeds for affected targets.
2. At least one targeted runtime/test path for changed subsystem is executed.
3. If status/coverage changed, update `Docs/status-v2.md` and `Docs/v2-feature-backlog.md`.
