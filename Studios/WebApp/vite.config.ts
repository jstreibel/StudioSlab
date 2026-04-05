import { copyFileSync, existsSync, mkdirSync, readFileSync } from 'node:fs'
import { basename, dirname, join, resolve } from 'node:path'
import { fileURLToPath } from 'node:url'
import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'
import type { Plugin, ResolvedConfig } from 'vite'

const wasmFiles = [
  'ising-workspace-wasm-sandbox.html',
  'ising-workspace-wasm-sandbox.js',
  'ising-workspace-wasm-sandbox.wasm',
] as const

const currentDirectory = dirname(fileURLToPath(import.meta.url))
const wasmSourceDir = resolve(currentDirectory, '../../cmake-build-webgl-wasm/Build/bin')

const contentTypeByExtension: Record<string, string> = {
  html: 'text/html; charset=utf-8',
  js: 'text/javascript; charset=utf-8',
  wasm: 'application/wasm',
}

function studioSlabWasmBridge(): Plugin {
  let resolvedConfig: ResolvedConfig

  const copyBuiltAssets = () => {
    const wasmOutputDir = resolve(resolvedConfig.root, resolvedConfig.build.outDir, 'wasm')
    mkdirSync(wasmOutputDir, { recursive: true })

    for (const fileName of wasmFiles) {
      const sourceFile = resolve(wasmSourceDir, fileName)
      if (!existsSync(sourceFile)) {
        resolvedConfig.logger.warn(
          `[studioslab-wasm-bridge] Missing ${fileName}. Build WasmIsingWorkspaceSandbox to enable the launch card.`,
        )
        continue
      }

      copyFileSync(sourceFile, join(wasmOutputDir, fileName))
    }
  }

  return {
    name: 'studioslab-wasm-bridge',
    configResolved(config) {
      resolvedConfig = config
    },
    configureServer(server) {
      server.middlewares.use((request, response, next) => {
        const requestPath = request.url?.split('?')[0] ?? ''
        if (!requestPath.startsWith('/wasm/')) {
          next()
          return
        }

        const fileName = basename(requestPath)
        if (!wasmFiles.includes(fileName as (typeof wasmFiles)[number])) {
          next()
          return
        }

        const sourceFile = resolve(wasmSourceDir, fileName)
        if (!existsSync(sourceFile)) {
          next()
          return
        }

        const extension = fileName.split('.').pop() ?? ''
        const contentType = contentTypeByExtension[extension]
        if (contentType) {
          response.setHeader('Content-Type', contentType)
        }

        response.end(readFileSync(sourceFile))
      })
    },
    closeBundle() {
      copyBuiltAssets()
    },
  }
}

export default defineConfig({
  base: './',
  plugins: [react(), studioSlabWasmBridge()],
})
