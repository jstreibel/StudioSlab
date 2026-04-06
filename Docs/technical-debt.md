# Technical Debt TODOs

## Reflection Catalog Registry V2 (2026-03-05)

- `TODO(registry-collision-policy)`: define explicit conflict policy for duplicate `InterfaceId` across catalog sources.
  - Current behavior: first seen interface wins during merged-catalog build.
  - Options: source priority list, conflict diagnostics UI, or hard error with source pair details.

- `TODO(registry-singleton-lifecycle)`: remove implicit global lifetime assumptions.
  - Current behavior: process-global singleton with mutable source callbacks.
  - Options: inject registry via app/lab composition root, add scoped lifetime, and codify ownership.

- `TODO(registry-thread-safety-contract)`: document/enforce threading contract for register/refresh/invoke.
  - Current behavior: no explicit lock/thread contract in registry type.
  - Options: UI-thread-only invariant with assertions, or mutex-protected multi-thread contract.

- `TODO(registry-refresh-cost)`: avoid unconditional `RefreshAll()` on every Schemes frame.
  - Current behavior: full refresh per draw for inspector and blueprint graph.
  - Options: dirty-flag refresh, interval throttling, or explicit refresh button with event-driven invalidation.

- `TODO(plots-inspector-unification)`: migrate Plots inspector to read through registry source APIs for parity with Schemes.

## Typesetting Bridge V1 (2026-03-11)

- `TODO(typesetting-math-validation-strategy)`: replace the current NanoTeX safety shim with an explicit invalid-math strategy.
  - Current behavior: `TypesettingService` uses a small command whitelist plus brace-balance check and falls back to text rendering before calling NanoTeX when input looks unsafe.
  - Why this needs discussion: "complete" behavior depends on product intent for arbitrary model notation. Options include a richer validator, a curated supported LaTeX subset, isolating NanoTeX parsing behind a safer boundary, or replacing NanoTeX for UI math rendering.

- `TODO(typesetting-nanotex-lifecycle)`: remove the process-lifetime NanoTeX runtime workaround when a stable ownership model exists.
  - Current behavior: `Graphics/LaTeX/LaTeX.cpp` initializes NanoTeX once and intentionally skips `tex::LaTeX::release()` because the bundled teardown path crashes in this environment.
  - Why this needs discussion: the correct fix may be upstream patching, local vendor patching, a different runtime lifetime contract, or moving the math backend behind a more isolated renderer service.

- `TODO(typesetting-selection-highlighting-pass)`: implement the first real selection/highlighting/hover treatment on top of the new measured rows in the Model tab.
  - Current behavior: row bounds and deterministic placement are in place, but the tab still uses only basic selectable rows and does not yet link hovered/selected math rows to a dedicated inspector flow.
  - Why this needs discussion: the next step should choose whether selection is row-based only, formula-segment based later, or tied immediately to model inspector/navigation semantics.

- `TODO(typesetting-cache-policy)`: revisit cache sizing and eviction after more UI surfaces adopt the service.
  - Current behavior: renderables are cached by content/style/DPI with a simple max-entry trim and lazy GL upload.
  - Why this needs discussion: a complete policy depends on expected scene size, atlas reuse goals, texture memory budget, and whether future tabs need prewarming or explicit invalidation hooks.

- `TODO(typesetting-nanotex-debug-logging)`: suppress or gate NanoTeX startup/debug logging in development and tests.
  - Current behavior: focused tests pass, but NanoTeX still emits verbose startup and glyph logs to stdout in this build.
  - Why this needs discussion: the clean fix is probably in third-party build/config flags rather than the service layer, and that should be handled once the repo decides how aggressively to patch vendored dependencies.
