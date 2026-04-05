# StudioSlab

StudioSlab is a CMake-based C++23 workspace for physics simulations, numerical experiments, visualization, and a scientific creation platform in progress.

## Repository Roles

- `Lib/`
  - shared reusable libraries, legacy/core code, and many domain/model implementations
- `Slab/`
  - shared V2 platform/editor/runtime landing zone
- `Studios/`
  - executable targets, standalone studies, CLI tools, legacy apps, and sandboxes
- `StudioSlab`
  - the current desktop workbench binary built from `Studios/LabV2/`

The repo intentionally serves both:
- a broad reusable toolbox for experiments and standalone targets
- a main platform/workbench path centered on `StudioSlab` / `LabV2`

For the placement rules and naming disambiguation, read `Docs/repo-architecture-overview.md`.

## Current Direction

- `LabV2` is the main editor/workbench path.
- shared V2 platform/editor/runtime code should live in `Slab/` when it is reused across targets
- reusable model/domain code still often belongs in `Lib/`
- browser/wasm work is currently a bounded platform spike, not a full `LabV2` port

## Getting Started

Read these first:
- `Docs/index.md`
- `Docs/status-v2.md`
- `Docs/repo-architecture-overview.md`
- `SPEC.md`

Install dependencies on Ubuntu:

```bash
./Scripts/install-deps.sh
```

Configure and build:

```bash
cmake -S . -B cmake-build-debug -DSTUDIOSLAB_CUDA_SUPPORT=ON
cmake --build cmake-build-debug --target Slab StudioSlab testsuite -j8
```

Useful targets:
- `Slab`: aggregate static library
- `StudioSlab`: current `LabV2` desktop workbench
- `testsuite`: Catch2-based tests
- `Studios/WebGL-WASM`: standalone browser/wasm sandboxes

## Browser / WASM

Minimal browser targets live under `Studios/WebGL-WASM/`.

Example:

```bash
emcmake cmake -S Studios/WebGL-WASM -B cmake-build-webgl-wasm
cmake --build cmake-build-webgl-wasm --target WebGLWasmSandbox WasmImGuiSandbox -j8
```

This is intentionally separate from the current desktop `Lib/Graphics` stack.

## Web App

A generic StudioSlab web frontend now lives under `Studios/WebApp/`.

It is intentionally local to that directory:
- local `package.json`
- local `node_modules`
- no root JS workspace
- no CMake integration

Run it with:

```bash
cd Studios/WebApp
npm install
npm run dev
```

Build it with:

```bash
cd Studios/WebApp
npm run build
```
