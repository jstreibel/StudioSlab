# Monitoring: LiveView vs Listeners (V2)

## Purpose

Capture the current architectural tradeoff for starting/stopping monitoring at runtime, without bloating task/runtime complexity.

## Current Runtime Reality

- `FNumericTaskV2` builds its subscription list once during initialization.
- There is no runtime API to inject/remove subscriptions on a running task.
- Passive monitor windows for `spi`, `rtor`, `r2tor`, `moldyn`, `xy`, and `ising` consume copied latest snapshots from `SessionLiveViewV2`.
- Metropolis `r2tor` monitor path still uses a model-specific snapshot listener.
- `SessionLiveViewV2` also exposes compatibility read leases, but monitor redraw paths should use snapshots.

## Why Runtime Subscription Injection Is Risky

Injecting listeners into a running task requires concurrent mutation of structures currently treated as static:

- task-side subscription storage (`FNumericTaskV2`)
- scheduler due-state (`FOutputSchedulerV2`)
- dispatcher iteration/lifecycle semantics (`OnRunStarted`, `OnRunFinished`)

Main risks:

- data races / iterator invalidation / lock-order bugs
- semantic ambiguity for late listeners (what counts as initial/final)
- hot-loop performance regressions from coarse locking

Assessment:

- complexity: medium-high
- risk: medium
- not recommended as first step for runtime monitor attach/detach

## Implemented Direction (Current Default)

For runtime start/stop of monitor windows, default to:

1. make monitor windows consume render data from `SessionLiveViewV2` snapshots
2. keep listener subscriptions static for deterministic analytics/transform pipelines
3. allow monitor UI to open/close independently of task subscription topology

This gives:

- low-to-medium migration complexity
- lower concurrency risk
- no mandatory full-state copy per update

## Copy/Performance Note

`SessionLiveView` snapshot publication should stay bounded:

- snapshot topic stores only latest copied frame (single-slot/latest-only semantics)
- copying can be consumer-gated so unread monitor streams do not add steady-state cost
- monitor renders last available snapshot when no newer version arrives

Full copies should remain explicit and opt-in in listeners that need snapshot persistence, transforms, or downstream replay.

## Role Split (Keep Both Abstractions)

- `LiveView`: telemetry/status + monitor-oriented latest-snapshot surfaces
- listeners: deterministic push hooks for scheduled analytics, side effects, persisted snapshots, and pipelines

These are complementary, not mutually exclusive.
