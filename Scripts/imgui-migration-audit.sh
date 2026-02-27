#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

echo "[imgui-migration-audit] root: ${ROOT_DIR}"
echo

echo "== Direct stack-layout API usage (outside 3rdParty) =="
if ! rg -n "ImGui::(BeginHorizontal|EndHorizontal|BeginVertical|EndVertical|Spring|SuspendLayout|ResumeLayout)" \
    Lib Studios --glob '!Lib/3rdParty/**'; then
    echo "(none)"
fi
echo

echo "== imgui_internal.h includes (outside 3rdParty) =="
if ! rg -n "imgui_internal\\.h" Lib Studios --glob '!Lib/3rdParty/**'; then
    echo "(none)"
fi
echo

echo "== Layout compat provider references =="
if ! rg -n "FImGuiLayoutCompat|GetProviderName|STUDIOSLAB_IMGUI_LAYOUT_USE_FORK" \
    CMakeLists.txt Lib Studios Docs --glob '!Lib/3rdParty/**'; then
    echo "(none)"
fi
