# V2 Model Coverage Matrix (Draft v0.1)

## Purpose

Guide V2 migration across model families without overfitting architecture to a single showcase.

This complements:
- a **north-star Study** (deep UX/presentation target)
- ongoing **coverage migration** (breadth across execution/data patterns)

## Status Legend

- `Legacy` = only V1 path
- `V2-H` = V2 headless run
- `V2-M` = V2 monitored (passive GL or equivalent)
- `V2-I` = V2 interactive control (live control inputs)
- `V2-S` = V2 sequenced/presentation-ready

## Priority Legend

- `P0` = highest leverage for architecture now
- `P1` = important next coverage
- `P2` = later / specialized

## Coverage Dimensions (what each model stresses)

- `Time`: `step`, `sim-time`, `open`
- `Space`: `R->R`, `R^2->R`, `particles`, `lattice`, `state-only`
- `Stoch`: deterministic vs stochastic/RNG
- `Ctrl`: live control injection pressure
- `View`: main visualization type
- `Det`: determinism/reproducibility pressure
- `Data`: outputs typically needed (`snap`, `hist`, `spec`, `diag`, etc.)

## Matrix

| Family / Example | Current | Priority | Time | Space | Stoch | Ctrl | View | Det | Data | Why It Matters Architecturally |
|---|---:|---:|---|---|---|---|---|---|---|---|
| KG `R->R` linear stepping (plane waves) | `V2-M` | `P0` | sim-time | `R->R` | no | medium | plot | high | snap/hist/spec | Already in V2; good base for analysis, control injection, sequencing |
| KG `R^2->R` field evolution | `V2-I` | `P0` | sim-time + open | `R^2->R` | no / optional | **high** | field render + plots | high | snap/hist/diag | Now has V2 headless + monitor + first live-control forcing prototype |
| SPI (Stochastic Path Integral) | `V2-M` | `P0` | sim-time | `R^2->R` | solver-specific | low/medium | section + field | medium | snap/hist/diag | Exercises ODE/time-aware V2 and 2D field visualization with lower UX entropy than KG2D |
| KG `R->R` Monte Carlo (Metropolis) | `V2-M` | `P1` | step / pseudo-time | state-only / `R->R` state | **yes** | low | telemetry / derived plots | high (RNG) | hist/diag | V2 monitored path now available via CLI and LabV2 |
| Molecular Dynamics (LJ / SoftDisk) | `V2-M` | `P1` | sim-time + open | particles | optional (Langevin) | high | particle scene + plots | high | traj/hist/diag | Baseline V2 headless + passive monitor now in CLI/LabV2; good foundation for controls and thermostats |
| Molecular Dynamics (Langevin / thermal baths) | `Legacy` | `P1` | sim-time + open | particles | **yes** | medium | particle scene + diagnostics | high | hist/diag | Combines stochasticity + real-time particle rendering + reproducibility pressure |
| XY lattice Monte Carlo | `V2-M` | `P1` | step / pseudo-time | lattice | **yes** | low/medium | lattice field + stats | high (RNG) | hist/diag/spec | V2 headless + passive monitor now wired (`Studios xy`, `LabV2` launcher) |
| Ising lattice Monte Carlo | `Legacy` | `P1` | step / pseudo-time | lattice | **yes** | low/medium | lattice field + stats | high (RNG) | hist/diag/spec | Completes lattice-family breadth and validates binary-spin dynamics in V2 |
| XY with kinetic term (time evolution prototype) | `Legacy/proto` | `P2` | sim-time | lattice | maybe | medium | lattice + spectra | medium/high | hist/spec | Hybrid semantics (lattice + time) useful once V2 control/sequence matures |
| Functional minimization workflows | `Legacy` | `P1` | iteration / pseudo-time | varied | no / maybe | low | convergence plots + field snapshots | high | hist/diag/snap | Tests transform pipelines where “run” is not traditional dynamical simulation |
| KG Langevin variants | `Legacy` | `P2` | sim-time | `R->R` / `R^2->R` | **yes** | medium | plots/fields | high | hist/spec/diag | Great later stress test for stochastic PDE + control + reproducibility |

## North-Star Study vs Coverage Portfolio

### North-Star Study (depth)

Use one deeply integrated target to drive editor and presentation features.

Recommended candidate:
- **Interactive KG `R^2->R` with external forcing `J(x^μ)`**
  - live field poke via mouse
  - passive monitors
  - camera/annotation tracks tied to simulation time
  - recorded control replay

### Coverage Portfolio (breadth)

Migrate representative families incrementally to validate architecture generality.

Rule of thumb:
- every new migration should validate at least one *new* stress dimension
- avoid migrating many models that all stress the same dimensions first

## Suggested Migration Order (Rolling, not rigid)

### Wave A (stabilize current V2 portfolio)
- KG `R->R` (`V2-M`) -> add interactive hooks + deeper analysis
- SPI (`V2-M`) -> improve parity and reuse infrastructure
- Metropolis (`V2-H`) -> add monitored/analysis-facing path

### Wave B (north-star enabler)
- KG `R^2->R` -> `V2-H` then `V2-M`
- introduce `LiveControl V2` for interactive forcing

### Wave C (broad dynamics coverage)
- Molecular Dynamics core (headless + monitored)
- thermal/Langevin MD variant

### Wave D (lattice / pseudo-time coverage)
- XY / Ising MC with V2 runs, streams, and diagnostics

### Wave E (presentation and sequencing pressure)
- sequence/control replay attached to the north-star and one additional model family

## Acceptance Criteria for a “Meaningful Migration”

A model family should not be considered “migrated” based on compilation only.

Minimum useful milestones:
1. `V2-H`: runs via `FNumericTaskV2` with tests
2. `V2-M`: passive view/monitor path in `Studios` or `LabV2`
3. `LiveData` integration: telemetry/status/session topics (or equivalent)
4. reproducibility notes documented (RNG/precision/runtime caveats)

## Notes / Open Questions

- Final name for the primary authored artifact (`Study`) is currently the recommended internal/product term.
- “Model” remains the scientific system/transform term.
- This matrix should be updated as new V2 slices land and as `LiveControl V2` and sequencing mature.
- MD V2 first-slice scope is captured in `Docs/md-v2-slice-scope.md`.
