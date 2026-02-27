#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

run_profile() {
    local label="$1"
    local build_dir="$2"
    local layout_flag="$3"

    echo "[imgui-provider-runtime-matrix] profile=${label}"
    cmake -S . -B "${build_dir}" \
        -DSTUDIOSLAB_CUDA_SUPPORT=OFF \
        -DSTUDIOSLAB_IMGUI_LAYOUT_USE_FORK="${layout_flag}"
    cmake --build "${build_dir}" --target Studios

    Scripts/imgui-runtime-smoke.sh "${ROOT_DIR}/Build/bin/SlabTests" "${label}"
}

run_profile "fork" "cmake-build-debug" "ON"
run_profile "compat" "cmake-build-debug-imgui-compat" "OFF"

echo "[imgui-provider-runtime-matrix] PASS"
