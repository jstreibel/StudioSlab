# V2 Feature Backlog (Compact, Draft v0.1)

## Purpose

Planning-oriented task list for V2 work.

Use together with:
- `Docs/status-v2.md` (current implementation snapshot)
- `Docs/v2-model-coverage-matrix.md` (breadth/coverage planning)
- `Docs/study-model-spec.md` and `Docs/sequence-control-spec.md` (product/UX architecture)

## Status / Priority Conventions

- Status: `todo`, `active`, `blocked`, `done`
- Priority: `P0` (now), `P1` (next), `P2` (later)
- Status precedence: progress notes + `Docs/status-v2.md`; seed items below include historical lineage.

## Current Recommended Milestone (`NS0`)

Goal: enable the first north-star interaction loop (field + monitor + live control).

## Progress Notes (2026-02-27)

- `KG2D-01` done: `KGR2toR` baseline V2 headless slice (`Studios kg2d` + tests).
- `KG2D-02` done: passive GL monitor path for `KGR2toR` baseline V2.
- `LC-01` foundation done: `LiveControl V2` topics/hub + LabV2 visibility panel.
- `KG2D-03` done: first control binding prototype (LabV2 control source -> hub topics -> KG2D external forcing source).
- `KG2D-03.5` done then removed from current LabV2 shell: prior KG2D monitor-side control source publisher path is no longer exposed as a dedicated panel/pre-publish surface.
- `CVG-00` done: Metropolis V2 now has a passive monitor path (`Studios metropolis --gl`, LabV2 launcher).
- `LAB-00` done: Data Browser polish (topic filters, selected-topic details for LiveData/LiveControl).
- `LAB-01` done: View management surface baseline (list/select/focus/close/retile).
- `LAB-02` done: Run Manager improvements baseline (task filters, grouped counters, progress/cursor table, clear actions).
- `CVG-01` done: MD V2 scoping draft captured in `Docs/md-v2-slice-scope.md`.
- `CVG-02` done: MD baseline `V2-H` and `V2-M` paths now wired in CLI/LabV2 (`Studios moldyn`, `Studios moldyn --gl`).
- `CVG-03` done: XY and Ising V2 lattice slices now have `V2-H` + `V2-M` paths in CLI/LabV2 (`Studios xy|ising`, optional `--gl` passive monitors).
- `CVG-03.5` done: SlabTests visual smoke commands now include XY/Ising monitor paths (`xy-monitor-smoke`, `ising-monitor-smoke`).
- `CVG-04` active: Functional minimization V2 first-slice scope drafted in `Docs/functional-minimization-v2-slice-scope.md`.
- `LAB-03` done: LabV2 top-level panel/window orchestration centralized in `FLabV2WindowManager` with registry-driven panel surfaces.
- `LAB-04` done: ImGui-wrapped Slab canvas event routing hardened (mouse hit-test routing, keyboard focus routing, persisted bounds/last-mouse bridge).
- `LAB-05` done: Plot window controls moved to viewport-attached toolbar + attached detail inspector (`Docs/plot-window-ux-contract.md`).

## Progress Notes (2026-03-01)

- `LAB-06` done: Simulation launcher naming/tooling cleanup in LabV2 (removed `V2` naming clutter, shortened per-example labels, added launcher action tooltips).
- `LAB-07` done: Passive monitor window UX hardening in LabV2 docking:
  - simulation monitors now default to plot-first composition with in-window controls
  - slab monitor windows render/input only in `Monitor` workspace
  - tab-switch artifacts and repeated zero-viewport warnings resolved in `Simulations`/`Schemes` workspaces

## Progress Notes (2026-03-02)

- `LP-00` first slice implemented:
  - generic scalar live-parameter binding helper (`LiveParameterControlV2`)
  - XY/Ising runtime controls now bind `Temperature` and `ExternalField` through `LiveControl` topics
  - const launch parameters are published as read-only control topics in the same namespace
  - LabV2 launcher can optionally publish XY/Ising live parameter values while simulations run
  - tests added for generic XY/Ising runtime binding path
- `LAB-08` done: LabV2 plot-overlay mouse routing fix and KG2D panel cleanup:
  - mouse events captured by ImGui overlays now stop propagation to underlying plot canvases
  - removed dedicated KG2D control-source panel and its default monitor-side pre-publish behavior from LabV2 shell

## Progress Notes (2026-03-04)

- `RV2-00` done (design/doc phase): Reflection V2 baseline contract locked.
  - indexed route added: `Docs/index-reflection-v2.md`
  - contract added: `Docs/reflection-v2-contract.md`
  - migration plan added: `Docs/reflection-v2-migration-plan.md`
  - decision locked: unified operation model (`Command|Query` kind), one invocation path, no heavy CQRS split
  - decision locked: `RestartRequired` parameter mutability for staged apply-on-restart semantics
- `RV2-01` done: adapter-first discovery path implemented in LabV2 Schemes over shared reflection catalog.
- `RV2-02` done: operation invoke dispatcher + command/query wiring + invocation policy checks.
- `RV2-03` done: first complex type slice (`slab.math.function.r_to_r`) with editor/codec/apply path.
- `RV2-04` done: CLI convergence first vertical slice via `Studios reflect` over same adapter/codecs/invoke path.
- `LAB-09` done: Schemes workspace now splits reflection metadata and blueprint graph concerns:
  - renamed old overloaded `Blueprints` metadata surface to `Interface Inspector`
  - added `Blueprint Graph` node-canvas panel with draggable nodes and interface-parameter-operation links
- `LAB-10` active (first interaction slice): Schemes blueprint graph now supports node-level reflection actions and richer reflection-state visibility:
  - operation lanes split into `Queries` and `Commands`
  - node badges expose mutability/exposure/live-vs-draft-vs-pending and operation policy metadata
  - hover quick actions invoke reflection operations from graph nodes (`Get`/`Set`/`Apply`/`Invoke`)
  - graph trace rail logs recent invoke outcomes and latency

## Progress Notes (2026-03-06)

- `LAB-10` progressed: Schemes graph now consumes a shared substrate graph document:
  - substrate node view-model integration in `Blueprint Graph` surface
  - edge rendering switched to substrate edges (`FromNodeId`/`ToNodeId`) instead of local ad-hoc reconstruction
  - canvas pan/grid state moved to substrate canvas state
- Graph Playground mode convergence progressed:
  - semantic/template/runtime/routing playground paths now use shared `FGraphDocumentV2` state
  - one substrate now carries common graph primitives across Schemes and all Graph Playground mode canvases
  - shared substrate canvas renderer now drives all Graph Playground modes plus Schemes `Blueprint Graph`
  - Schemes graph dropped its right-click dummy plot/artist creation menu while preserving policy-driven node graph usage
- Graph Playground first user-story implementation progressed:
  - Template -> Runtime instantiation action landed (with bound/unbound + edge diagnostics surface)
  - Runtime mode now toggles between reflection snapshot and instantiated runtime graph
  - Routing mode now sources/selects runtime endpoints and executes connect through an invocation path before mutating graph edges
  - routing operation telemetry and runtime-instantiation session state persistence added

## Progress Notes (2026-03-13)

- `LAB-12` done: Model-tab ambient semantic environment baseline:
  - added model-level `BaseVocabulary` preset selection and ambient entry resolution
  - added preset baseline for `core_math`, `classical_mechanics`, and `relativistic_field_theory`
  - semantic origin reporting now distinguishes local definitions, base vocabulary, and assumptions
  - Model workspace now exposes inspectable readonly `Base Vocabulary` as a dedicated semantic surface
  - notation conventions use preview LaTeX templates instead of bare command tokens
- `LAB-13` done: Model-tab semantic navigation and transactional authoring pass:
  - cross-highlighting and reference navigation now connect vocabulary, definitions, relations, assumptions, and inspector diagnostics
  - model-level semantic status/classification summary now reports health, unresolved symbols, pending assumptions, and inferred model character
  - draft semantic delta preview compares canonical vs preview semantic state before apply
  - draft-only assumptions/materializations are navigable during preview
  - direct create flow exists for new local definitions and new local relations
  - Model workspace moved from one stacked panel to separate dockable windows for vocabulary, definitions, relations, editor, assumptions, and inspector

## Progress Notes (2026-03-14)

- `LAB-14` done: Model semantic graph plot surface and interaction pass:
  - `Model Layer` now exposes `Semantic Graph -> Open in Plots`
  - graph projection comes from `FModelSemanticOverviewV2`
  - graph is read-only, selection-linked, and rendered on `Plot V2`
  - hover HUD, edge inspection, click activation, neighborhood-hop controls, label toggle, and fit-to-graph are in place
- `LAB-15` done: Plot V2 host-control parity first slice:
  - added a migration-plan doc and updated the Plot2D V2 scheme doc to match the actual architecture
  - LabV2 V2-plot hosts now provide attached toolbar/detail controls instead of being render-only shells
  - V2 artist z-order is now reflected and editable, restoring layer-control parity needed by the host overlay
- `RZ-00` done: ODE realization contract freeze implemented in code.
- `RZ-01` done: ODE descriptor extraction and readiness gating implemented from canonical model semantics.
- `RZ-02` done: model-level ODE initial-condition semantics implemented and validated.
- `RZ-03` done: descriptor-driven ODE runtime bridge implemented:
  - explicit first-order runtime system builder
  - `FSimulationRecipeV2` + `FStepperSessionV2` handoff
  - explicit numeric binding map for parameters and initial symbols
- ODE realization validation baseline now matches the plan:
  - harmonic oscillator: positive
  - damped harmonic oscillator: positive
  - Klein-Gordon: negative boundary check
- post-`RZ-03` follow-up is now:
  - minimal numeric-binding authoring/configuration story
  - minimal LabV2 launch path from an ODE-ready model

### `P0` `RV2-00` — Reflection V2 contract freeze — done
- Define `Interface`/`Parameter`/`Operation` schema contracts.
- Define operation invocation result shape and policy enums.
- Define type id baseline and adapter boundary from legacy interface seed.

### `P0` `RV2-01` — Reflection adapter read-only path — done
- Build adapter projection from legacy `FInterface`/`FParameter`.
- Expose discovery metadata in LabV2 Schemes (read-only first).
- Do not change legacy runtime semantics in this phase.

### `P1` `RV2-02` — Reflection operation invocation — done
- Add unified operation invoke path with policy enforcement (`ThreadAffinity`, `RunStatePolicy`, `SideEffectClass`).
- Wire first host/runtime command and query operations.

### `P1` `RV2-03` — Complex parameter type slice — done
- Add first complex function descriptor type adapter (`r_to_r` or `r2_to_r`).
- Provide minimal editor surface and validation path.

### `P1` `RV2-04` — CLI convergence over reflection codecs — done
- Route one vertical CLI slice through reflection codecs while preserving legacy compatibility.

### `P1` `RV2-05` — Reflection id normalization + alias policy
- Formalize agent/user-facing id policy for legacy snake_case vs V2 kebab-case.
- Decide and implement alias/compat behavior for mixed-id inputs across UI/CLI/API.

### `P0` `LC-00` — `LiveControl V2` spec
- Define control topics/streams, source kinds, timing domains, and binding semantics.

### `P0` `KG2D-00` — `KG R^2->R` V2 slice scoping
- Pick exact legacy path and migration seam (likely `LeadingDelta`-related, but scoped).

### `P0` `KG2D-01` — `KG R^2->R` V2 headless slice (`V2-H`)
- `FSimulationRecipeV2` + `FNumericTaskV2` path, tests included.

### `P0` `KG2D-02` — `KG R^2->R` passive monitor (`V2-M`)
- Field view + telemetry via `LiveData V2`.

### `P0` `LC-01` — Minimal `LiveControl V2` implementation
- Wall-clock input stream(s), topic/hub basics, typed control values.

### `P0` `KG2D-03` — First control binding prototype (“poke field”)
- Mouse/UI control -> external forcing `J(x^\\mu)` (best-effort wall-clock path).
- done as prototype (`LabV2`/monitor-side source + CLI flags for KG2D), with dedicated panel path later removed from current LabV2 shell.

### `P0` `LP-00` — Live runtime parameters (first slice)
- reference: `Docs/live-parameters-v2-slice-scope.md`
- add mutability/exposure policy for runtime parameters
- implement generic scalar binding path to `LiveControl V2`
- first adopters: XY + Ising (`Temperature`, `ExternalField`)

## Parallel Track (keep one item active max)

### `P1` `CVG-00` — Metropolis `V2-M` — done
- Add a monitored V2 path (telemetry/diagnostics view), not only headless.

## Follow-up Tracks (next)

### Study / Sequence
- `P1` `SEQ-00` — Sequence runtime spec refinement (tracks, clocks, domain mapping)
- `P1` `SEQ-01` — Minimal sequence runtime (camera + annotations)
- `P1` `ST-00` — In-memory `Study` object skeleton

### LabV2 Editor Tool Surfaces
- `P1` `LAB-00` — Data Browser polish (filtering, grouping, row details) — done
- `P1` `LAB-01` — View management surface (focus/close/retile) — done
- `P1` `LAB-02` — Run Manager improvements (grouping/error clarity) — done
- `P1` `LAB-03` — Centralized panel/window orchestration contract — done
- `P1` `LAB-04` — Sub-window event routing contract and wrapper hardening — done
- `P1` `LAB-05` — Plot window control surface UX hardening — done
- `P1` `LAB-10` — Schemes blueprint graph execution interactions
  - support operation invoke from graph nodes/pins (not only inspector panel buttons)
  - add clear command/query affordances in-graph
  - status: active (`node-level invoke` and command/query lane clarity done; deeper pin wiring remains)
- `P1` `LAB-11` — Schemes graph persistence
  - persist per-interface node positions/pan/viewport preferences
  - restore layout state on workspace startup
- `P1` `LAB-12` — Model tab semantic environment baseline — done
  - model-level ambient `BaseVocabulary`
  - origin-aware semantic inspection
  - inspectable ambient vocabulary section in the Model tab
- `P1` `LAB-13` — Model tab semantic navigation + authoring surfaces — done
  - semantic cross-highlighting and clickable provenance/dependency navigation
  - model-level semantic health/classification summary
  - draft semantic delta preview and draft-only object inspection
  - direct creation of local definitions/relations
  - dockable Model workspace surfaces
- `P1` `LAB-14` — Model semantic graph plot surface — done
  - plot-based graph projection from semantic overview data
  - click-through selection into the Model workspace
  - keyboard/pointer interaction for neighborhood, labels, and fit
- `P1` `LAB-15` — Plot V2 host-control parity — done
  - attached toolbar/detail controls on the V2 host
  - reflected V2 artist z-order and richer artist-list metadata
- `P1` `LAB-16` — Plot V2 shared host + remaining artist migration
  - extract the V2 plot host out of `LabV2WindowManager.cpp`
  - port remaining legacy plot artists (`Labels`, `XHair`, `History`, `R2Section`, `R2toR`) into V2-native paths

### Model -> Realization Descent
- `P1` `RZ-00` — ODE realization contract freeze — done
  - plan reference: `Docs/ode-realization-descent-plan.md`
  - define one realization-facing descriptor derived from canonical `Model V2` semantics
  - gate on semantic readiness: no unresolved symbols, no blocking errors, no required pending assumptions
- `P1` `RZ-01` — ODE descriptor extraction — done
  - derive time coordinate, state variables, parameters, observables, and selected relations
  - validate first on harmonic oscillator / damped oscillator
- `P1` `RZ-02` — ODE initial-condition semantics — done
  - add explicit initial-condition slots for the ODE slice only
  - keep boundary/control semantics out of this slice
- `P1` `RZ-03` — First runtime bridge — done
  - plan reference: `Docs/ode-realization-descent-plan.md`
  - handoff reference: `Docs/handoff-ode-realization-rz03.md`
  - build one descriptor-driven recipe/session/stepper path for oscillator-family models
  - keep diagnostics explicit and state ordering deterministic
- `P1` `RZ-04` — ODE launch/binding follow-up
  - expose the minimal numeric scalar-binding story required by the runtime bridge
  - add one LabV2 launch path from an ODE-ready model into that bridge

### Coverage Portfolio
- `P1` `CVG-01` — MD V2 scoping — done
- `P1` `CVG-02` — MD `V2-H` -> `V2-M` — done
- `P1` `CVG-03` — XY/Ising V2 lattice slices (`V2-H` -> `V2-M`) — done
- `P2` `CVG-04` — Functional minimization V2 slice

### Platform Targets
- `P2` `PLAT-00` — Web/wasm feasibility spike
  - plan reference: `Docs/platform-wasm-feasibility.md`
  - define minimal headless/plot-only target surface
  - document required backend and dependency split from desktop-native build graph

## Working Rule (Entropy Control)

Keep at most:
- 1 north-star item active
- 1 coverage item active
- 1 editor/tooling item active

If ambiguity grows, stop and resolve spec/contracts before continuing implementation.
