# Molecular Dynamics V2 Slice Scope (Draft v0.1)

## Status (2026-02-27)

- `MD-01` implemented.
- `MD-02` implemented (baseline passive monitor).

## Purpose

Define a safe first migration slice for Molecular Dynamics into V2 while keeping risk low and reuse high.

This slice is about:
- proving particle-state support in V2 runtime/data paths
- validating monitoring needs for particle simulations
- avoiding a full MD architecture rewrite in one wave

## Legacy Inputs (Observed)

Primary existing pieces:
- `Lib/Models/MolecularDynamics/MolecularDynamicsRecipe.*`
- `Lib/Models/MolecularDynamics/VerletStepper.*`
- `Lib/Models/MolecularDynamics/Hamiltonians/*`
- `Lib/Models/MolecularDynamics/Monitor.*`
- `Studios/MolecularDynamics/*`

## Scoping Decision (Recommended)

Do not port all MD variants at once.

Split into two incremental phases:

### Phase M0 (`MD Core Headless`)
- one deterministic MD path in V2 (`SoftDisk` or `LennardJones`)
- finite-run first (`V2-H`)
- basic telemetry/status and test coverage

### Phase M1 (`MD Passive Monitor`)
- passive particle monitor window wired through `LiveData V2`
- no interactive control input yet
- no thermostat/Langevin path yet

## First Concrete Target

Recommended first scenario:
- deterministic `SoftDisk` (no stochastic thermostat)
- moderate particle count
- finite step budget

Reason:
- simpler reproducibility and debug behavior than thermal variants
- still exercises particle rendering and pairwise dynamics pressure

## Minimal Deliverables

### `MD-01` (`V2-H`)
- `Studios` subprogram for MD V2 headless run
- recipe/task path based on V2 runtime (`FSimulationRecipeV2` + `FNumericTaskV2`)
- tests covering:
  - cursor progression
  - finite value checks for particle state
  - deterministic behavior for fixed seed/config (where applicable)

### `MD-02` (`V2-M`)
- passive GL monitor for particle positions + basic telemetry
- LabV2 launcher entry
- final-frame visibility and clean run-finished behavior

## Explicit Deferrals

Not in this first slice:
- Langevin/thermal bath MD
- live control injection (mouse poke/forces)
- sequence/timeline integration
- deep analysis outputs (RDF, advanced spectra, batching pipelines)

## Key Pressure Points

Watch these during implementation:
- particle-state exposure contract (`zero-copy lease` vs copied display buffer)
- monitor rendering cost for larger `N`
- deterministic replay under floating-point + threading constraints
- compatibility seam with legacy MD hamiltonian/stepper internals

If any pressure point gets unclear, stop and tighten contracts before broadening scope.

## Acceptance Criteria

`MD-01`:
- headless run completes via V2 task
- tests pass in `testsuite [V2]`

`MD-02`:
- passive monitor renders particles from live session data
- launchable from CLI and LabV2
- build and V2 tests remain green
