# WebGL WASM Sandboxes

Independent browser/wasm experiments for StudioSlab.

Current targets:
- `WebGLWasmSandbox`: direct WebGL2 triangle smoke.
- `WasmImGuiSandbox`: minimal Dear ImGui + OpenGL ES 3 / WebGL2 smoke.

These targets are intentionally independent from the desktop `Lib/Graphics` stack. They are the bounded place to validate browser toolchain, deployment, and UI assumptions before attempting any wider backend split.

Reusable build glue lives in `cmake/StudioSlabWasm.cmake`.

## Build (Emscripten, standalone)

```bash
emcmake cmake -S Studios/WebGL-WASM -B cmake-build-webgl-wasm
cmake --build cmake-build-webgl-wasm --target WebGLWasmSandbox WasmImGuiSandbox -j8
```

Expected outputs:
- `cmake-build-webgl-wasm/Build/bin/webgl-wasm-sandbox.html`
- `cmake-build-webgl-wasm/Build/bin/imgui-wasm-sandbox.html`
- matching `.js` and `.wasm` artifacts

## Run

```bash
emrun --no_browser cmake-build-webgl-wasm/Build/bin/webgl-wasm-sandbox.html
emrun --no_browser cmake-build-webgl-wasm/Build/bin/imgui-wasm-sandbox.html
```

Open the reported URL in a browser to view the selected sandbox.
