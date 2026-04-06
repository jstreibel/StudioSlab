# Platform Target Feasibility: Web/WASM

## Snapshot

- Date: `2026-04-03`
- Scope assessed: adding wasm targets for the current `StudioSlab` / `LabV2` codebase
- Result: **high complexity** for full `Slab`/`LabV2` wasm port (`not simple`)
- Independent bootstrap status:
  - reusable Emscripten target helper added at `cmake/StudioSlabWasm.cmake`
  - standalone WebGL2, ImGui, shared-workspace-shell, and browser-local Ising sandboxes live under `Studios/WebGL-WASM/`
  - browser targets remain intentionally decoupled from the current desktop `Lib/Graphics` stack while reusing the shared `Slab/Graphics/Window/V2` shell/layout layer
  - locally revalidated on `2026-04-03` with `Emscripten 3.1.6` (`emcmake` configure, all three targets build; prior `emrun --no_browser` serving validation remains from `2026-04-01`)

## Why It Is High Complexity

1. Build graph is desktop-native by default.
- Root CMake resolves `GLEW`, `GLUT`, `OpenGL`, optional CUDA, and `SFML` (`CMakeLists.txt`).

2. Graphics stack is strongly tied to native OpenGL + desktop windowing.
- `Lib/Graphics` compiles GLFW/SFML platform backends plus OpenGL legacy/modern paths in one static library.
- Backends request OpenGL 4.6 desktop contexts (`GLFWPlatformWindow`, `SFMLSystemWindow`).

3. Rendering code still relies on fixed-function OpenGL.
- Multiple plot/window paths use `glMatrixMode`, `glOrtho`, `glBegin/glEnd` immediate mode.
- That does not map directly to WebGL2 without significant renderer migration.

4. Third-party linkage is desktop-centric.
- ImGui static target currently compiles `imgui_impl_glut`, `imgui_impl_glfw`, `imgui_impl_opengl3` and links `GLU`.
- Graphics also requires `Fontconfig`, `CairoMM`, `PangoMM` (NanoTeX path).

5. Runtime/task model uses native threads in shared paths.
- Common V2 runner utilities spawn `std::thread`; wasm threading support would require dedicated build/runtime constraints.

## Practical Conclusion

Adding a real wasm target is a platform split project, not a small build toggle.

Minimum viable path would require:
- a wasm-specific backend selection strategy (likely browser canvas + Emscripten main loop),
- a renderer compatibility layer (or strict modern-GL-only subset),
- dependency partitioning to exclude desktop-only libs from wasm builds,
- a thread policy for wasm (`single-thread` first or `pthreads` with deployment constraints).

## Current Architecture Decision

Use `ImGui` for the first browser GUI path.

Why:
- `LabV2` is already ImGui-centric
- the repo vendors ImGui Emscripten examples
- current Nuklear/NanoGUI integration is incomplete compared with ImGui

Near-term implementation shape:
- keep browser targets as standalone studios under `Studios/WebGL-WASM/`
- keep reusable browser build glue in `cmake/StudioSlabWasm.cmake`
- do not force current desktop `Lib/Graphics` to compile under Emscripten yet

Mid-term direction:
- add a browser backend analogous to `GLFW`, but only after desktop-only dependencies are partitioned out of the current graphics stack
- keep shared browser backend code in shared platform layers, not inside one studio target

V2 render seam:
- `FPlot2DWindowHostV2` should depend on `IPlotRenderBackendV2`, not one concrete renderer
- browser-safe Plot2D work should land as another `IPlotRenderBackendV2` implementation rather than as a `GLFW` hack

## Recommended Next Step

Use backlog item `PLAT-00` for a bounded feasibility spike:
- define one smallest target (`headless numerics` or `single-plot viewer`),
- produce a dedicated wasm toolchain CMake preset and dependency matrix,
- avoid promising full LabV2 parity in the first spike.

## Independent Bootstrap (Now Available)

Isolated wasm/browser targets now exist:
- `WebGLWasmSandbox`
- `WasmImGuiSandbox`
- `WasmWorkspaceSandbox`
- `WasmIsingWorkspaceSandbox`

Path:
- `Studios/WebGL-WASM/`

Intent:
- validate Emscripten + browser rendering basics
- validate a minimal GUI path (`ImGui`) without coupling to the current desktop graphics stack
- validate that the extracted V2 workspace shell/layout contract can be reused in a browser-hosted sample without bringing over desktop slab-window/render backends
- validate one bounded browser-local model workspace (`Ising Metropolis`) without requiring the full desktop simulation/runtime shell
- establish reusable target glue for future browser sandboxes

Standalone configure/build:
- `emcmake cmake -S Studios/WebGL-WASM -B cmake-build-webgl-wasm`
- `cmake --build cmake-build-webgl-wasm --target WebGLWasmSandbox WasmImGuiSandbox WasmWorkspaceSandbox WasmIsingWorkspaceSandbox -j8`
- `emrun --no_browser cmake-build-webgl-wasm/Build/bin/webgl-wasm-sandbox.html`
- `emrun --no_browser cmake-build-webgl-wasm/Build/bin/imgui-wasm-sandbox.html`
- `emrun --no_browser cmake-build-webgl-wasm/Build/bin/workspace-wasm-sandbox.html`
- `emrun --no_browser cmake-build-webgl-wasm/Build/bin/ising-workspace-wasm-sandbox.html`

If both sandboxes are served concurrently, launch them with distinct `emrun --port` values to avoid the default-port collision.

This still does **not** imply that `Slab`/`LabV2` are close to a full browser port.
It only de-risks toolchain, packaging, and one bounded GUI path.

## Smallest Target Candidates

1. Candidate A: `wasm-headless-core`
- Scope: compile `Core + Utils + Math` with `Headless` backend only.
- Output: wasm module that runs one numeric task and prints terminal summary.
- Effort: low-to-medium.
- Risk: medium (toolchain flags and C++23/library compatibility).

2. Candidate B: `wasm-single-plot`
- Scope: browser canvas + ImGui + one plot path.
- Output: interactive viewer in browser.
- Effort: high.
- Risk: very high (OpenGL/WebGL migration + backend split).

3. Candidate C: `wasm-labv2-shell`
- Scope: LabV2 workspace shell in browser.
- Output: browser-hosted editor shell.
- Effort: very high.
- Risk: very high (platform/window/input architecture differences).

Recommended first candidate: **Candidate A (`wasm-headless-core`)**.

## PLAT-00 Concrete Spike Plan

Step 1: Build Surface Partition
- Task: define a `WASM_MINIMAL` compile profile that excludes `Lib/Graphics`, `Lib/3rdParty` desktop GL glue, and all Studio executables requiring graphics.
- Deliverable: one dependency map doc + one CMake target list for wasm.
- Effort: `0.5-1.0 day`.
- Risk: `low`.

Step 2: Emscripten Toolchain Preset
- Task: add CMake preset/toolchain invocation for `emcmake` with reproducible flags (`-sWASM=1`, strict warnings, no threads initially).
- Deliverable: documented configure/build command and a successful configure log.
- Effort: `0.5 day`.
- Risk: `medium`.

Step 3: Compile `wasm-headless-core`
- Task: build a minimal executable/library path that starts `Core`, uses `Headless` backend, runs a tiny numeric smoke.
- Deliverable: generated `.wasm` + `.js/.html` harness and successful startup output.
- Effort: `1.0-2.0 days`.
- Risk: `high`.

Step 4: Runtime Smoke + CI Hook
- Task: add one deterministic smoke command for wasm output validation (local script and optional CI job skeleton).
- Deliverable: `Scripts/` smoke runner and docs in `Docs/index-testing.md`.
- Effort: `0.5-1.0 day`.
- Risk: `medium`.

Step 5: Decision Gate (`Go/No-Go`)
- Task: evaluate whether to proceed to any graphics/web canvas work.
- Deliverable: short report with blocker list and phase-2 recommendation.
- Effort: `0.25 day`.
- Risk: `low`.

Estimated PLAT-00 total: **`2.75-4.75 engineer-days`**.

## Go/No-Go Criteria

`Go` to phase 2 only if all are true:
- `wasm-headless-core` builds reproducibly from clean checkout.
- one numeric smoke runs to completion in browser or node runtime.
- no mandatory graphics dependency leaks into the minimal target.

`No-Go` (stop and redesign) if any are true:
- unavoidable hard dependency on desktop OpenGL/SFML in core runtime path,
- repeated toolchain breakage from required C++ features,
- wasm runtime stability not sufficient for deterministic smoke.

## Phase 2 (Out Of Spike Scope)

If `Go`, next bounded step is **not** LabV2 full port.
Preferred next step: isolated `wasm-single-plot` prototype with a modern draw path only, no fixed-function fallback.
