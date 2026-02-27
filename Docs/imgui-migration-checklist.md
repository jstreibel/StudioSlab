# ImGui Fork -> Upstream Migration Checklist

## Scope
- Migrate StudioSlab from forked ImGui (`Lib/3rdParty/imgui`) to upstream ImGui.
- Keep Blueprints usable during migration.
- Preserve behavior first; improve features (tabs/docking/etc.) after parity.

## Constraints
- No Blueprint regressions are allowed in migration waves.
- Fork remains the default provider until upstream parity is proven.
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

## Wave Plan

### Wave A - Baseline and Safety Net
- Freeze baseline visual scenarios for Blueprints and Lab/LabV2.
- Keep a short manual validation script for repeated checks.
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
- Add explicit provider selection (`fork`, `upstream`) at build/config level.
- Keep default as `fork`.
- Exit criteria:
  - both providers compile under same app interfaces.

### Wave E - Upstream Bring-Up
- Point ImGui to upstream.
- Resolve API drifts (`imgui_internal.h` touches, node editor compatibility).
- Exit criteria:
  - upstream provider reaches parity gates.

### Wave F - Fork Decommission
- Keep fallback path for one release cycle.
- Remove fork once parity is stable and documented.
- Exit criteria:
  - fork not required for production Blueprint workflows.

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
