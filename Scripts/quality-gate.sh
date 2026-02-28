#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

RUN_RUNTIME=0
if [[ "${1:-}" == "--with-runtime" ]]; then
    RUN_RUNTIME=1
fi

echo "[quality-gate] audit"
Scripts/imgui-migration-audit.sh

echo "[quality-gate] configure/build upstream-default"
cmake -S . -B cmake-build-debug -DSTUDIOSLAB_CUDA_SUPPORT=OFF -DSTUDIOSLAB_IMGUI_LAYOUT_USE_FORK=OFF
cmake --build cmake-build-debug --target Slab
cmake --build cmake-build-debug --target Studios

echo "[quality-gate] configure/build legacy-flag (deprecated)"
cmake -S . -B cmake-build-debug-imgui-compat -DSTUDIOSLAB_CUDA_SUPPORT=OFF -DSTUDIOSLAB_IMGUI_LAYOUT_USE_FORK=ON
cmake --build cmake-build-debug-imgui-compat --target Slab
cmake --build cmake-build-debug-imgui-compat --target Studios

if [[ ${RUN_RUNTIME} -eq 1 ]]; then
    echo "[quality-gate] runtime smoke matrix"
    Scripts/imgui-provider-runtime-matrix.sh
fi

echo "[quality-gate] PASS"
