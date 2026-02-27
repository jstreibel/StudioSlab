#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
IMGUI_DIR="${ROOT_DIR}/Lib/3rdParty/imgui"
UPSTREAM_TAG="${1:-v1.91.0}"
BUILD_DIR="${2:-cmake-build-debug-upstream-spike}"

if [[ ! -e "${IMGUI_DIR}/.git" ]]; then
    echo "Expected ImGui submodule at ${IMGUI_DIR}" >&2
    exit 1
fi

if ! git -C "${IMGUI_DIR}" diff --quiet || ! git -C "${IMGUI_DIR}" diff --cached --quiet; then
    echo "ImGui submodule has local changes. Clean it before running the spike." >&2
    exit 1
fi

ORIGINAL_SHA="$(git -C "${IMGUI_DIR}" rev-parse HEAD)"

cleanup() {
    git -C "${IMGUI_DIR}" checkout "${ORIGINAL_SHA}" >/dev/null 2>&1 || true
}
trap cleanup EXIT

git -C "${IMGUI_DIR}" remote add upstream https://github.com/ocornut/imgui.git >/dev/null 2>&1 || true
git -C "${IMGUI_DIR}" fetch --tags upstream
git -C "${IMGUI_DIR}" checkout "${UPSTREAM_TAG}"

cd "${ROOT_DIR}"

cmake -S . -B "${BUILD_DIR}" \
    -DSTUDIOSLAB_CUDA_SUPPORT=OFF \
    -DSTUDIOSLAB_IMGUI_LAYOUT_USE_FORK=ON

cmake --build "${BUILD_DIR}" --target Slab
cmake --build "${BUILD_DIR}" --target Studios

echo "Upstream ImGui spike succeeded with tag ${UPSTREAM_TAG}."
