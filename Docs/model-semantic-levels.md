# Model Semantic Levels

## Purpose

Record the minimum semantic stratification needed to keep Model graphs ontologically coherent when the same symbol appears in multiple mathematical framings.

Use this when:
- a declared observable/state/field appears algebraically and gets inferred as scalar-like
- deciding whether a semantic mismatch is a real bug or a level ambiguity
- extending the canonical Model graph toward realization-safe semantics

## Read Order

1. `Docs/status-v2.md`
2. `Docs/model-semantic-graph-layering.md`
3. this file
4. `Docs/ode-realization-descent-plan.md`

## Core Rule

Keep these axes separate:
- `Kind`: what the object is
- `Carrier/Level`: where it lives semantically
- `Representation`: how it is currently written or projected

Current minimal examples:
- `Kind`: `Parameter`, `Coordinate`, `StateVariable`, `Field`, `Observable`, `Operator`, `Equation`
- `Carrier/Level`: `ParameterSpace`, `StateSpace`, `TrajectoryDomain`
- `Representation`: coordinate expression, operator expression, pulled-back trajectory signal, plotted neighborhood node

Do not let local algebraic syntax collapse these axes into one flat role.

## Harmonic Oscillator Example

Use two compatible views:
- state-space observable: `H : X × P -> R`
- trajectory-level signal: `E = H ∘ <xi, lambda> : T -> R`

Interpretation:
- `x`, `p` are state coordinates on `X`
- `m`, `k` are parameters in `P`
- `H(x, p; m, k)` is a coordinate expression for a state-space observable
- `E(t)` is the pulled-back energy along a trajectory

So the ambiguity is not `observable versus scalar parameter`.
The real distinction is `state-space observable versus trajectory-level observable`.

## Current Implementation Slice

Current code now preserves resolved declared kinds when algebraic syntax is too weak to decide level cleanly:
- `Coordinate` is not demoted to scalar-like just because it appears algebraically
- `StateVariable` is not demoted to scalar-like just because it appears algebraically
- `Field` is not demoted to scalar-like/state-like just because it appears algebraically
- `ObservableSymbol` is not demoted to scalar-like/state-like just because it appears algebraically

Current scope:
- removes the false harmonic-oscillator `obs.energy` mismatch
- keeps the one-level ODE descent path semantically cleaner

Current non-goal:
- this does not yet store explicit `Carrier/Level` metadata in the canonical graph

## Near-Term Policy

- treat ambiguous algebraic usage as representation evidence, not final ontology
- preserve stronger declared canonical kinds when resolution already knows them
- prefer a future `level_ambiguity` diagnostic over a wrong `declared_inferred_mismatch`
- let realization consume only canonical subgraphs whose kinds and levels are coherent
