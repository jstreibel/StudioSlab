cmake_minimum_required(VERSION 3.22)

project(WebGLWasmSandbox LANGUAGES C CXX)

if (NOT EMSCRIPTEN)
    message(STATUS "Skipping WebGLWasmSandbox target (requires Emscripten toolchain).")
    return()
endif()

add_executable(WebGLWasmSandbox
    main.cpp
)

set_target_properties(WebGLWasmSandbox PROPERTIES
    OUTPUT_NAME "webgl-wasm-sandbox"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/bin"
    SUFFIX ".html"
)

target_compile_features(WebGLWasmSandbox PRIVATE cxx_std_20)

target_link_options(WebGLWasmSandbox PRIVATE
    "-sWASM=1"
    "-sUSE_WEBGL2=1"
    "-sMIN_WEBGL_VERSION=2"
    "-sMAX_WEBGL_VERSION=2"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sASSERTIONS=1"
    "-sNO_EXIT_RUNTIME=1"
    "-sENVIRONMENT=web"
)

message(NOTICE "Added executable WebGLWasmSandbox (Emscripten/WebGL2)")
