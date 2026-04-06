# StudioSlab WebApp

Local React frontend workspace for StudioSlab.

This app is intentionally self-contained under `Studios/WebApp`:
- local `package.json`
- local `node_modules`
- no root JS workspace
- no CMake coupling

## Stack

- React 19
- Redux Toolkit
- Tailwind CSS
- Vite 6
- TypeScript

## Commands

```bash
cd Studios/WebApp
npm install
npm run dev
```

Build a production bundle:

```bash
cd Studios/WebApp
npm run build
```

Preview the built bundle locally:

```bash
cd Studios/WebApp
npm run preview
```

## Launch Cards

The landing page includes:
- an `Ising Model` launch card that opens the existing wasm sandbox at `wasm/ising-workspace-wasm-sandbox.html`
- a `GitHub` launch card that opens the public repository page at `https://github.com/jstreibel/StudioSlab`

## WASM Bridge

During `vite` dev, the app serves that file directly from:
- `cmake-build-webgl-wasm/Build/bin`

During `vite build`, the same html/js/wasm artifact set is copied into the web app output.

If the launch card target is missing, build the wasm sandbox first:

```bash
cmake --build cmake-build-webgl-wasm --target WasmIsingWorkspaceSandbox -j8
```

## Current Scope

The initial scaffold is a generic StudioSlab web shell:
- Redux-backed workbench state
- Tailwind + CSS theming
- browser-oriented landing surface for future StudioSlab web features

This is separate from the bounded wasm sandboxes in `Studios/WebGL-WASM`.

## GitHub Pages Deployment

The repository now includes `.github/workflows/deploy-webapp-pages.yml`.

On every push to `main`, that workflow:
- initializes the `Lib/3rdParty/imgui` submodule
- installs Emscripten `3.1.6`
- builds `WasmIsingWorkspaceSandbox`
- builds this web app
- deploys `Studios/WebApp/dist` to GitHub Pages

One-time GitHub-side setup:
- open `Settings` for the `jstreibel/StudioSlab` repository
- open `Pages`
- set `Build and deployment` -> `Source` to `GitHub Actions`

Default published URLs:
- site: `https://jstreibel.github.io/StudioSlab/`
- Ising sandbox: `https://jstreibel.github.io/StudioSlab/wasm/ising-workspace-wasm-sandbox.html`
