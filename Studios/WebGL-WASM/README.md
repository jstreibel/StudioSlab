# WebGL WASM Sandbox

Independent wasm/WebGL2 experiment target.

Goals:
- validate browser rendering/toolchain path without coupling to `Slab`/`LabV2` runtime,
- keep first wasm milestone small and functional.

## Build (Emscripten, standalone)

```bash
emcmake cmake -S Studios/WebGL-WASM -B cmake-build-webgl-wasm
cmake --build cmake-build-webgl-wasm --target WebGLWasmSandbox -j8
```

Expected outputs:
- `cmake-build-webgl-wasm/Build/bin/webgl-wasm-sandbox.html`
- matching `.js` and `.wasm` artifacts

## Run

```bash
emrun --no_browser cmake-build-webgl-wasm/Build/bin/webgl-wasm-sandbox.html
```

Open the reported URL in a browser to see the animated triangle.
