#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SLAB_TESTS_BIN="${1:-${ROOT_DIR}/Build/bin/SlabTests}"
LABEL="${2:-current}"

if [[ ! -x "${SLAB_TESTS_BIN}" ]]; then
    echo "Missing SlabTests binary at ${SLAB_TESTS_BIN}" >&2
    exit 1
fi

STAMP="$(date +%Y%m%d-%H%M%S)"
LOG_DIR="${ROOT_DIR}/Build/logs/imgui-runtime-smoke"
mkdir -p "${LOG_DIR}"
LOG_FILE="${LOG_DIR}/${STAMP}-${LABEL}.log"

run_test() {
    local name="$1"
    shift
    echo "== [${LABEL}] ${name} $*" | tee -a "${LOG_FILE}"
    "${SLAB_TESTS_BIN}" "${name}" "$@" 2>&1 | tee -a "${LOG_FILE}"
}

echo "[imgui-runtime-smoke] label=${LABEL}" | tee "${LOG_FILE}"
echo "[imgui-runtime-smoke] binary=${SLAB_TESTS_BIN}" | tee -a "${LOG_FILE}"

run_test panel-plot-and-gui --seconds 1
run_test row-heavy-layout --seconds 1
run_test spi-live-monitor-mock --seconds 1 --steps 200
run_test xy-monitor-smoke --seconds 1 --steps 200 -L 32
run_test ising-monitor-smoke --seconds 1 --steps 200 -L 32

echo "[imgui-runtime-smoke] PASS log=${LOG_FILE}" | tee -a "${LOG_FILE}"
