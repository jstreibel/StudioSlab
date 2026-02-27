# ImGui Upstream Parity Log

## Purpose
- Track runtime parity deltas with upstream as default.
- Keep findings concrete and reproducible across profiles.

## Profiles
- `upstream-default`: `-DSTUDIOSLAB_IMGUI_LAYOUT_USE_FORK=OFF`
- `legacy-flag-on` (deprecated): `-DSTUDIOSLAB_IMGUI_LAYOUT_USE_FORK=ON`
- `upstream-spike`: temporary upstream checkout via `Scripts/imgui-upstream-runtime-spike.sh`

## Automated Smoke Commands
- `Scripts/imgui-provider-runtime-matrix.sh`
- `Scripts/imgui-upstream-runtime-spike.sh v1.91.0`

Each run writes logs under `Build/logs/imgui-runtime-smoke/`.

## Entry Template
- Date:
- Profile:
- Build:
- Runtime tests:
  - `panel-plot-and-gui`
  - `row-heavy-layout`
  - `spi-live-monitor-mock`
  - `xy-monitor-smoke`
  - `ising-monitor-smoke`
- Result: `Pass` / `Fail`
- Deltas observed:
- Follow-up fix commit:

## Baseline
- Date: 2026-02-27
- Profile: `upstream-default`
- Build: `Slab + Studios` green
- Runtime tests:
  - all five smoke tests pass with `--seconds 1` and `--steps 200` for monitor tests.
- Result: `Pass`
- Deltas observed:
  - Repeated non-fatal warning on shutdown:
    - `TODO: fix problematic font deletion ... was not deleted.`
- Follow-up fix commit: _pending_

## Recent Runs
- Date: 2026-02-27
- Profile: `upstream-default`
- Build: `cmake-build-debug`
- Runtime tests:
  - all five tests pass via `Scripts/imgui-provider-runtime-matrix.sh`
- Result: `Pass`
- Deltas observed:
  - same non-fatal font deletion warning.
- Follow-up fix commit: _pending_

- Date: 2026-02-27
- Profile: `legacy-flag-on`
- Build: `cmake-build-debug-imgui-compat`
- Runtime tests:
  - all five tests pass via `Scripts/imgui-provider-runtime-matrix.sh`
- Result: `Pass`
- Deltas observed:
  - same non-fatal font deletion warning.
- Follow-up fix commit: _pending_

- Date: 2026-02-27
- Profile: `upstream-spike (v1.91.0)`
- Build: `cmake-build-debug-upstream-spike`
- Runtime tests:
  - all five tests pass via `Scripts/imgui-upstream-runtime-spike.sh`
- Result: `Pass`
- Deltas observed:
  - same non-fatal font deletion warning.
- Follow-up fix commit: _pending_

- Date: 2026-02-27
- Profile: `full validation run (all profiles)`
- Build:
  - `cmake-build-debug`
  - `cmake-build-debug-imgui-compat`
  - `cmake-build-debug-upstream-spike`
- Runtime tests:
  - audit + build + provider matrix + upstream runtime spike all pass
- Result: `Pass`
- Deltas observed:
  - same non-fatal font deletion warning.
- Evidence:
  - `Build/validation-logs/upstream-default-migration-20260227-184143/summary.txt`
  - `Build/validation-logs/upstream-default-migration-20260227-184143/status.tsv`
  - runtime smoke logs under `Build/logs/imgui-runtime-smoke/`.
- Follow-up fix commit: _pending_
