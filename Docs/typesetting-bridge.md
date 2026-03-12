# Typesetting Bridge V1

## Purpose

This note documents the current UI-facing typesetting path for scientific text and math in `StudioSlab`.

It is intentionally narrow:
- what the service abstracts
- how text and math are routed
- what is cached and owned
- how ImGui panels should consume it
- what the `Model` panel currently assumes

This is a V1 infrastructure note, not a final typography spec.

## Code Map

- Typesetting service:
  - `Lib/Graphics/Typesetting/TypesettingService.h`
  - `Lib/Graphics/Typesetting/TypesettingService.cpp`
- ImGui adapter:
  - `Lib/Graphics/Typesetting/ImGuiTypesetting.h`
  - `Lib/Graphics/Typesetting/ImGuiTypesetting.cpp`
- NanoTeX runtime wrapper:
  - `Lib/Graphics/LaTeX/LaTeX.cpp`
- Current LabV2 integration:
  - `Studios/LabV2/LabV2WindowManager.cpp`
  - `Studios/LabV2/LabV2WindowManager.h`
  - `Studios/LabV2/LabV2WindowManagerModelPanels.cpp`
- Current model notation/types:
  - `Slab/Core/Model/V2/ModelTypesV2.h`
  - `Slab/Core/Model/V2/ModelNotationV2.h`
  - `Slab/Core/Model/V2/ModelSeedsV2.h`

## UI-Facing Contract

The rest of the UI should ask one service:
- measure this content
- resolve a cached renderable for this content
- draw it in ImGui without knowing which backend produced it

Current shared request/response concepts:
- `FTypesetRequest`: content kind, content string, style
- `FTypesetStyle`: font pixel size, DPI scale, optional font file
- `FTypesetMetrics`: width, height, baseline, line height
- `FTypesetRenderable`: metrics, CPU alpha mask, optional GL texture, backend tag

## Backend Routing

Current V1 routing:
- plain text: FreeType/freetype-gl path
- math: NanoTeX path when the expression passes the current safety gate
- math fallback: text renderer when NanoTeX is rejected or throws

Current intent:
- keep one UI-facing abstraction
- allow mixed strategy internally later
- avoid direct NanoTeX or FreeType calls from panel code

Current limitation:
- simple math snippets are still commonly routed as math rather than opportunistically downgraded to cheaper plain text

## Cache And Ownership

Cache key fields currently include:
- content kind
- content string
- style token
- font pixel size
- DPI scale

Current cache behavior:
- cached renderables store metrics and CPU alpha-mask data
- GL texture upload is lazy and happens on first draw
- textures are owned by the cached renderable entry
- cache trimming is a simple max-entry eviction by last-use serial

Ownership/lifetime notes:
- `FTypesettingService` owns cache entries and any uploaded textures
- cache clear/trim destroys GL textures for evicted entries
- NanoTeX runtime is initialized once and intentionally kept alive for process lifetime in this environment

## ImGui Consumption Pattern

Panels should use the adapter helpers rather than the raw service where practical:
- `ImGuiMeasureTypeset`
- `ImGuiTypeset`
- `ImGuiTypesetText`
- `ImGuiTypesetMath`
- `ImGuiTypesetSelectableRow`
- `ImGuiTypesetLabelValue`

Current adapter responsibilities:
- normalize style from current ImGui font size and framebuffer DPI
- reserve deterministic layout space
- draw through current ImGui draw list
- return useful row bounds and hover/press state for selection-ready UI

## Model Panel V1 Behavior

Current `Model` panel usage:
- `Definitions` and `Relations` rows show only the typeset formula
- detailed metadata stays in `Details`
- hover previews an item immediately
- click pins the current selection
- `Details` reports whether focus came from hover preview or pinned selection
- `Definitions` and `Relations` have an internal vertical splitter
- `Notation Scratchpad` sits below the main content and has its own vertical splitter

Current layout intent:
- keep deterministic row bounds for later selection/highlighting work
- avoid panel-wide scrolling in normal use
- allow internal child scrolling where detail volume exceeds pane height

## Manual Validation

Minimum manual checks after touching this surface:
- open `Model`
- verify seeded Harmonic Oscillator and Klein-Gordon rows render readable math
- verify first open does not rerasterize every frame after warm-up
- verify row hover and pinned selection update `Details` correctly
- verify row highlight bounds match hit area
- verify `Definitions` / `Relations` splitter drags cleanly
- verify scratchpad splitter drags cleanly
- verify main panel does not degrade into one large scrolling surface in normal window sizes

## Known Gaps

Outstanding design debt is tracked in:
- `Docs/technical-debt.md`

The most relevant open items are:
- NanoTeX validation strategy
- NanoTeX runtime lifecycle cleanup
- cache policy refinement
- debug logging suppression
- deeper selection/highlighting semantics
