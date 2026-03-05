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
