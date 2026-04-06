#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

UPSTREAM_TAG="${1:-v1.91.0}"
BUILD_DIR="${2:-cmake-build-debug-upstream-spike}"

Scripts/imgui-upstream-spike.sh "${UPSTREAM_TAG}" "${BUILD_DIR}"
Scripts/imgui-runtime-smoke.sh "${ROOT_DIR}/Build/bin/SlabTests" "upstream-${UPSTREAM_TAG}"

echo "[imgui-upstream-runtime-spike] PASS tag=${UPSTREAM_TAG}"
