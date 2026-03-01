# StudioSlab Product Direction (Draft v0.1)

## Purpose

This document captures the product direction for `StudioSlab` as a scientific creation environment.
It is a living draft intended to align architecture, UX, and V2 refactor choices.

## Product Identity

`StudioSlab` is a scientific analogue of a real-time creation engine/editor.

- `Slab` = engine/platform/runtime/tooling foundation
- `Lab` = editor/workbench (scientific control plane)
- `Studios` = authored studies / examples / domain-specific experiences

This mirrors the intended dual meaning:
- **Studio**: lights, camera, action, authored presentation, timing, choreography
- **Lab/Slab**: rigor, instruments, repeatability, controlled experiments, solid foundations

## Core Thesis

StudioSlab is not only a solver framework and not only a plotting tool.

It is a studio where users can:
- create or import mathematical data
- run models/simulations (finite or open-ended)
- interact with live systems in real time
- visualize and present results
- compose workflows with nodes/blueprints
- persist, replay, and publish results locally or remotely

## Primary User Output

The primary authored output is a **Study**.

Examples:
- `Atomic nucleus scattering`
- `Driven Klein-Gordon field response`
- `Lennard-Jones thermalization`

A Study may include:
- one or more models/transforms
- parameters and initial/boundary conditions
- views/monitors/layout
- annotations and narrative text
- camera choreography tied to simulation time
- interaction bindings (e.g. mouse-driven forcing source)
- reproducibility metadata

## Product Positioning (Aspirational)

Target combination:
- Mathematica/Matlab breadth of mathematical functionality
- Unreal-style interactive authoring/editor workflows
- game-like real-time performance for live monitoring and interaction
- scientific reproducibility and provenance

This is intentionally ambitious. The architecture must preserve separation of concerns to make it tractable.

## Architectural Consequences

The product direction implies:
- **Data-first** architecture (not simulation-first)
- Simulations are a class of **Transform**
- Views/monitors are **passive**
- Interaction is a first-class **ControlSource**
- Execution and presentation are decoupled
- Time domains must be explicit (`step`, `simulation`, `wall-clock`, `sequence`)

## Near-Term Product Strategy

Continue the V2 boundary refactor while using `LabV2` as the editor/workbench path:
- keep `Studios/Lab` frozen (legacy reference)
- grow `Studios/LabV2` as the new editor shell
- keep shared V2 platform/editor infrastructure in `Slab/`
- keep model/domain implementations near `Lib/Models`

## North-Star and Breadth (Both Matter)

A single north-star study is useful, but not sufficient.

Recommended strategy:
1. **North-star Study**
- one high-value interactive/presentable target (e.g. interactive Klein-Gordon forcing + sequencing)

2. **Coverage Portfolio**
- migrate multiple existing model families to V2 to stress the architecture broadly
- use each model family as an architectural probe (ODE/PDE/MC/MD/minimization/stochastic)

This avoids overfitting the platform to one showcase while preserving a clear UX target.

## Current Alignment (Status)

For current implementation snapshot precedence, use `Docs/status-v2.md`.

Already aligned with this direction:
- Numerics V2 runtime (`Task/Recipe/Session`)
- Live Data V2 (topics/hub + session live views)
- Passive monitors (`Studios` V2 and `LabV2`)
- `LabV2` as V2 observability + launcher shell

Missing but clearly implied:
- expanded `LiveControl V2` beyond foundation (recording/replay and sequence-time integration)
- `Study` object model
- sequence/presentation system
- richer data/asset model and provenance

## Working Definition (for now)

StudioSlab is a real-time, reproducible scientific studio for authoring **Studies** from mathematical data, models, simulations, controls, and visual presentations.
