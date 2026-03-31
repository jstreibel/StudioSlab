cmake_minimum_required(VERSION 3.22)

project(StudioSlabWasmSandboxes LANGUAGES C CXX)

get_filename_component(STUDIOSLAB_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
list(APPEND CMAKE_MODULE_PATH "${STUDIOSLAB_REPO_ROOT}/cmake")
include(StudioSlabWasm)

if (NOT EMSCRIPTEN)
    message(STATUS "Skipping StudioSlab wasm sandboxes (requires the Emscripten toolchain).")
    return()
endif()

add_executable(WebGLWasmSandbox
    main.cpp
)
studioslab_configure_wasm_target(WebGLWasmSandbox
    OUTPUT_NAME "webgl-wasm-sandbox")

studioslab_add_imgui_wasm_app(WasmImGuiSandbox
    OUTPUT_NAME "imgui-wasm-sandbox"
    SOURCES
        imgui_main.cpp)

message(NOTICE "Added wasm sandboxes: WebGLWasmSandbox, WasmImGuiSandbox")
