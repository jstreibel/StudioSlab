# ImGui Fork -> Upstream Migration Checklist

## Scope
- Migrate StudioSlab from forked ImGui (`Lib/3rdParty/imgui`) to upstream ImGui.
- Keep Blueprints usable during migration.
- Preserve behavior first; improve features (tabs/docking/etc.) after parity.

## Constraints
- No Blueprint regressions are allowed in migration waves.
- Upstream ImGui is the default provider.
- Legacy fork path is deprecated and only kept as an emergency fallback toggle.
- Migration must be reversible at each wave.

## Current Coupling Snapshot
- Fork-specific stack-layout API usage in Blueprints:
  - `ImGui::BeginHorizontal`, `ImGui::EndHorizontal`
  - `ImGui::BeginVertical`, `ImGui::EndVertical`
  - `ImGui::Spring`
- Current call sites: `222` (Lab + Prototype Blueprints trees).
- Internal API coupling:
  - `imgui_internal.h` usage in Blueprints and graphics modules.
  - Blueprint splitter helper depends on `SplitterBehavior(...)`.

## Acceptance Gates (Global)
- `cmake --build <build-dir> --target Slab` green.
- `cmake --build <build-dir> --target Studios` green.
- Blueprint editor parity:
  - node rendering/layout unchanged,
  - link creation/deletion unchanged,
  - context actions unchanged,
  - pane splitter behavior unchanged.
- Lab/LabV2 UI still renders and interacts correctly.
- Manual smoke script: `Docs/validation/imgui-blueprints-smoke.md`.

## Current Status
- Wave A: `Green`
- Wave B: `Green`
- Wave C: `Green`
- Wave D: `Green`
- Wave E: `Green`
- Wave F: `Green`
- Extra hardening: Blueprint `SplitterBehavior(...)` dependency moved behind `FImGuiSplitterCompat`.
- Extra hardening: Blueprint pane now shows active layout provider (`fork-stack-layout` vs `compat-fallback`).
- Extra hardening: removed redundant `imgui_internal.h` includes from Blueprint widgets.
- Extra hardening: `imgui_internal.h` use reduced to 2 include sites (`FImGuiFrameCompat` + `FImGuiSplitterCompat`) tracked by audit script.
- Spike result: upstream ImGui `v1.91.0` compiles and runs smoke tests with the fallback layout provider.
- Submodule migration complete:
  - `.gitmodules` points to `https://github.com/ocornut/imgui.git`
  - `Lib/3rdParty/imgui` pinned to upstream `v1.91.0` (`8199457a7`)
- Runtime parity tooling added:
  - `Scripts/imgui-runtime-smoke.sh`
  - `Scripts/imgui-provider-runtime-matrix.sh`
  - `Scripts/imgui-upstream-runtime-spike.sh`
  - `Docs/validation/imgui-upstream-parity-log.md`
- Runtime parity status: upstream default, legacy-flag build, and upstream spike (`v1.91.0`) smoke runs are green.

## Wave Plan

### Wave A - Baseline and Safety Net
- Freeze baseline visual scenarios for Blueprints and Lab/LabV2.
- Keep a short manual validation script for repeated checks.
- Keep a migration audit script for dependency tracking (`Scripts/imgui-migration-audit.sh`).
- Exit criteria:
  - baseline scenarios documented and reproducible.

### Wave B - Layout Compatibility Seam (fork-backed)
- Introduce a single Blueprint-facing layout adapter.
- Route Blueprint stack-layout calls through the adapter.
- Do not change runtime behavior.
- Exit criteria:
  - no direct Blueprint calls to fork stack-layout APIs remain,
  - build + baseline checks pass.

### Wave C - Local Stack Layout Ownership
- Implement StudioSlab-owned stack-layout implementation behind adapter.
- Keep fork provider active while validating parity.
- Exit criteria:
  - adapter works with local implementation in parity mode,
  - baseline checks pass.

### Wave D - Provider Switch Infrastructure
- Add explicit provider selection at build/config level.
- Exit criteria:
  - both providers compile under same app interfaces.

### Wave E - Upstream Bring-Up
- Point ImGui to upstream.
- Resolve API drifts (`imgui_internal.h` touches, node editor compatibility).
- Keep an automated spike path for repeatability (`Scripts/imgui-upstream-spike.sh`).
- Exit criteria:
  - upstream provider reaches parity gates.

### Wave F - Fork Decommission
- Switch repository default to upstream ImGui submodule.
- Deprecate fork toggle and keep it only for emergency fallback.
- Exit criteria:
  - upstream default is green in build and runtime smoke,
  - fork is no longer referenced as default anywhere.

## Rollback Rules
- If any parity gate fails in a wave, revert only that wave and keep previous provider path.
- Never remove fallback until the next wave is verified green.

## Trackers
- Open risks:
  - internal ImGui API drift,
  - node editor compatibility with selected upstream version,
  - splitter behavior parity.
- Status fields per wave:
  - `Not Started`, `In Progress`, `Green`, `Blocked`.
