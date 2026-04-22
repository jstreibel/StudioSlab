# Platforms, Backends, and Modules V2

## Purpose

Define the next V2 boundary for runtime composition and capability selection.

This document exists because the repo already has:
- a legacy global backend/module system under `Lib/Core/Backend/*`
- one proven V2 capability seam in `Plot2D V2`
- growing need for clearer vocabulary around platform hosts, GUI, rendering, networking, and audio

It is a direction and migration contract, not a claim that the full V2 stack is already implemented.

## Current Baseline

Legacy baseline:
- `FBackendManager`
- `FBackend`
- `FGraphicBackend`
- `FSlabModule`

Current strengths:
- simple startup model
- easy string-based registration
- enough flexibility for legacy `Headless`, `GLFW`, and `SFML`

Current limits:
- global singleton composition
- stringly-typed lookup
- platform host and capability backend concerns are conflated
- module responsibilities are too broad and too shallow at the same time
- GUI/runtime/platform coupling is implicit

Existing V2 precedent:
- `Plot2D V2` already has a small capability-scoped backend seam through `IPlotRenderBackendV2`
- this is the model to generalize, not the legacy global backend manager

## Canonical Terms

### `PlatformHost`

Owns process-facing runtime hosting concerns.

Responsibilities:
- startup / shutdown lifetime
- event loop or frame loop
- system windows or browser canvas ownership when applicable
- input/event pump
- graphics/device/context bootstrap when applicable
- wall-clock/frame timing hooks
- host-level thread/runtime policy hooks

Examples:
- `HeadlessPlatformHostV2`
- `GLFWPlatformHostV2`
- `SDLPlatformHostV2`
- `SFMLPlatformHostV2`
- `BrowserPlatformHostV2`

Rule:
- in V2, `GLFW`, `SDL`, `SFML`, browser, and headless should be described as `PlatformHost`s, not generic `Backend`s

### `Backend`

A capability-scoped interchangeable implementation behind an explicit interface.

Examples:
- plot render backend
- general rendering backend
- task execution backend
- typesetting backend
- asset-store backend
- audio I/O backend
- HTTP transport backend
- WebSocket transport backend

Rule:
- one application may use several backends at the same time
- a backend should implement one capability family, not own the whole app runtime

### `Module`

A registration/composition bundle.

A module may:
- register service factories
- register backend factories
- register adapters and connectors
- contribute default configuration/profile fragments

A module should not:
- own the global application loop
- silently create the platform host
- replace CMake/library boundaries as the main architectural split

Rule:
- V2 `Module` means composition unit, not “entire subsystem in one object”

### `Service`

A typed runtime object consumed by the application or another subsystem.

Examples:
- `IPlotRenderBackendV2` instance
- `ITaskExecutionBackendV2` instance
- `IHttpServerV2` instance
- `IAudioDeviceBackendV2` instance

### `Connector`

A higher-level import/export/remote integration endpoint built over services/transports.

Examples:
- dataset sync endpoint
- remote compute endpoint
- telemetry bridge
- websocket-backed study collaboration endpoint

### `Composition Root`

The app-local place where a runtime profile is assembled.

Examples:
- `Studios/CLI`
- `Studios/LabV2`
- `Studios/WebGL-WASM/*`

The composition root chooses:
- platform host
- module set
- concrete backends
- default policies

It should receive shared builders/helpers from `Slab`, but remain explicit in the app.

## Placement Rules

### `Slab/Core/Composition/V2/`

Lives here:
- composition context / runtime context types
- typed service registry
- backend descriptor vocabulary
- module descriptor vocabulary
- app-profile assembly helpers

Does not live here:
- OpenGL/WebGL/Vulkan calls
- GUI toolkit bindings
- HTTP protocol code
- JACK or audio device code

This is the composition layer, not the capability layer.

### `Slab/Core/Platform/V2/`

Lives here:
- platform-host interfaces
- headless host contracts
- generic lifecycle contracts independent of one graphics toolkit
- host-facing timing / shutdown / policy abstractions

### `Slab/Graphics/Platform/V2/`

Lives here:
- concrete graphical platform hosts
- system-window adapters
- native event translation bridges
- browser canvas host adapters when graphics/windowing are involved

Examples:
- `GLFWPlatformHostV2`
- `SDLPlatformHostV2`
- `SFMLPlatformHostV2`
- browser-hosted graphical shell adapters

### `Slab/Graphics/Window/V2/`

Lives here:
- workspace shell
- workspace layout
- hosted-surface abstractions
- desktop/editor window orchestration contracts

Does not live here:
- OS event loops
- OpenGL/WebGL render backends
- GUI toolkit implementations

### `Slab/Graphics/GUI/V2/`

Lives here:
- GUI runtime contracts
- GUI context contracts
- immediate GUI adapters
- retained GUI state/widget trees
- GUI draw submission bridges

This is low-level GUI infrastructure, not the `LabV2` panel/tool surface layer.

### `Slab/Graphics/Rendering/V2/`

Lives here:
- generalized rendering contracts
- immediate command emission APIs
- retained scene/resource structures
- draw-list / command-list vocabulary
- render-queue vocabulary
- pass graph / frame graph / multi-pass orchestration
- backend implementations (`OpenGL`, `WebGL2`, `Vulkan`, `Recording`, ...)

### `Slab/Network/V2/`

Lives here:
- low-level network transports
- HTTP server/client abstractions
- WebSocket server/client abstractions
- network session/pump helpers

### `Slab/Audio/V2/`

Lives here:
- audio graph/device abstractions
- stream transport contracts
- real-time audio backend interfaces
- JACK/PortAudio/etc. implementations

### `Slab/Core/Connectors/V2/`

Lives here:
- higher-level import/export/remote connectors
- endpoints that combine persistence/runtime/network concerns
- study/data sync adapters
- remote compute connectors
- telemetry bridges

Rule:
- transports live under `Network`
- product-facing integration endpoints live under `Connectors`

### `Studios/*`

Lives here:
- app composition roots
- target-local platform/backend selection
- bounded proving-ground integrations
- executable-specific orchestration

## Direct Answers For Key Capability Areas

### GUI: Immediate and Retained

Immediate GUI should live under:
- `Slab/Graphics/GUI/V2/Immediate/`
- backend-specific bindings under `Slab/Graphics/GUI/V2/Backends/ImGui/`, `.../Nuklear/`, etc.

Retained GUI should live under:
- `Slab/Graphics/GUI/V2/Retained/`

Immediate GUI responsibilities:
- frame-scoped widget submission
- input focus/capture integration
- toolkit-specific draw-data generation

Retained GUI responsibilities:
- widget tree
- layout tree
- persistent state model
- styling and focus/navigation model
- optional diff/rebuild logic

Rule:
- immediate and retained GUI belong to the same GUI subsystem, not to platform hosts and not to `LabV2` panel code

Rule:
- `LabV2` panels remain editor tool surfaces built on top of GUI/runtime services; they should not become the GUI runtime itself

### General Rendering System

A more powerful renderer should live under:
- `Slab/Graphics/Rendering/V2/`

Recommended internal split:
- `Immediate/`
  - immediate convenience API emitting render commands
- `Commands/` or `DrawLists/`
  - backend-neutral render command vocabulary
- `Scene/`
  - retained scene objects, cameras, materials, resources, visibility inputs
- `Queues/`
  - sorting, batching, submission queues
- `FrameGraph/`
  - render targets, passes, dependencies, multi-pass orchestration
- `Backends/`
  - concrete API/device implementations
- `Adapters/`
  - optional bridges to external scene/render ecosystems

Rule:
- immediate APIs, retained scene structures, draw lists, render queues, and multi-pass are all rendering concerns and should live in one renderer subsystem with clear sublayers

Rule:
- domain renderers such as `Plot2D V2` may keep domain-local backend seams until there is proven value in lifting common abstractions into `Graphics/Rendering/V2`

### OSG / VSG-Class Integrations

If the repo grows a retained multi-pass renderer rich enough to host OSG/VSG-like integrations:
- adapters should live under `Slab/Graphics/Rendering/V2/Adapters/`
- they should implement or bridge the core rendering contracts
- they should not be the initial defining abstraction for the rendering layer

Rule:
- do not design the first rendering core around one external engine’s object model
- define StudioSlab-facing contracts first, then adapt external systems where useful

### HTTP Server and WebSockets

Low-level transport/runtime should live under:
- `Slab/Network/V2/Http/`
- `Slab/Network/V2/WebSocket/`

Examples:
- `IHttpServerV2`
- `IHttpClientV2`
- `IWebSocketServerV2`
- `IWebSocketClientV2`

If a higher-level feature uses them for product workflows:
- that feature-facing connector should live under `Slab/Core/Connectors/V2/`

Examples:
- remote telemetry connector
- browser study-sync connector
- remote compute job connector

### JACK Audio

JACK should live under:
- `Slab/Audio/V2/Backends/Jack/`

The generic contracts it implements should live under:
- `Slab/Audio/V2/`

Examples:
- `IAudioDeviceBackendV2`
- `IAudioGraphExecutorV2`
- `IAudioStreamEndpointV2`

Rule:
- `Jack` should be treated as one concrete audio backend, not as a generic top-level module category in V2

## Naming Rule For V2

Use these names in new V2 docs and code:
- `PlatformHost`
- `Backend`
- `Module`
- `Service`
- `Connector`
- `CompositionRoot`

Avoid in new V2 naming:
- using `Backend` to mean the whole app platform host
- using `Module` to mean “any subsystem singleton”

Migration note:
- legacy names such as `FBackend`, `FGraphicBackend`, and `FSlabModule` may remain while adapters exist
- do not rename the legacy world first; introduce the V2 vocabulary at the new boundary

## Migration Strategy

The migration should be bridge-first, not big-bang and not pure wrapper theater.

### Principle

Build green-field V2 contracts, then bridge proven legacy capabilities into them where doing so reduces migration risk.

### What “bridge-first” means here

Do:
- define new V2 interfaces and descriptors
- wrap legacy systems where new V2 consumers need them
- migrate one consumer at a time
- keep old startup paths operational until replacements are proven

Do not:
- rewrite every legacy backend/module before a V2 consumer exists
- create giant universal wrapper classes that preserve all old semantics forever
- force `Lib/*` subsystems to move physically into `Slab/*` before their V2 boundaries are stable

## Recommended Migration Waves

### `BM-00` Vocabulary + doc lock

- add this planning contract
- use `PlatformHost` terminology for new V2 work

### `BM-01` Composition seed

Add:
- `Slab/Core/Composition/V2/*`

First outputs:
- typed service registry
- backend/module descriptors
- one explicit runtime context object

### `BM-02` Platform host boundary

Add:
- `Slab/Core/Platform/V2/*`

First outputs:
- `IPlatformHostV2`
- `HeadlessPlatformHostV2`
- lifecycle/timing contracts

### `BM-03` Legacy platform bridge

Bridge:
- current `FBackend` / `FGraphicBackend` implementations into `PlatformHost V2`

Goal:
- let one V2 composition root boot through legacy `GLFW` or `Headless` without rewriting all platform code first

### `BM-04` Plot2D remains the first capability backend exemplar

Keep:
- `IPlotRenderBackendV2` as the precedent seam

Extend:
- composition/profile-driven backend selection

### `BM-05` Task execution backend

Add:
- `ITaskExecutionBackendV2`

First backends:
- `InlineTaskExecutionBackendV2`
- `LocalThreadsTaskExecutionBackendV2`

Initial bridge:
- adapt current `FTaskManager` where useful

### `BM-06` GUI runtime extraction

Add:
- `Slab/Graphics/GUI/V2/*`

First target:
- explicit immediate GUI contracts over current ImGui path

### `BM-07` Network and audio seeds

Add:
- `Slab/Network/V2/*`
- `Slab/Audio/V2/*`

First targets:
- HTTP/WebSocket transport seed
- JACK audio backend seed only if an actual consumer slice needs it

## Anti-Goals

- do not design one god-object `IBackendV2`
- do not collapse platform, GUI, rendering, networking, and audio into one registration table without typed contracts
- do not force all domains to migrate simultaneously
- do not over-generalize the renderer before at least two real subsystems need the same abstraction

## Immediate Practical Rule

For the next slices:
- treat `GLFW` / `SDL` / `SFML` / browser / headless as `PlatformHost`s
- treat render/audio/network/execution implementations as `Backend`s
- treat registration bundles as `Module`s
- keep app assembly explicit in `Studios/*`
- use legacy bridges where they reduce risk and keep momentum
