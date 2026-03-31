include_guard(GLOBAL)

include(CMakeParseArguments)

get_filename_component(STUDIOSLAB_WASM_REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(STUDIOSLAB_WASM_DEFAULT_SHELL_FILE
    "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/examples/libs/emscripten/shell_minimal.html")

function(studioslab_configure_wasm_target target)
    if (NOT TARGET ${target})
        message(FATAL_ERROR "studioslab_configure_wasm_target(): unknown target '${target}'.")
    endif()

    if (NOT EMSCRIPTEN)
        message(FATAL_ERROR
            "studioslab_configure_wasm_target(): target '${target}' requires the Emscripten toolchain.")
    endif()

    set(options)
    set(oneValueArgs OUTPUT_NAME SHELL_FILE)
    set(multiValueArgs
        COMPILE_DEFINITIONS
        INCLUDE_DIRECTORIES
        LIBRARIES
        LINK_OPTIONS
        PRELOAD_FILES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (ARG_OUTPUT_NAME)
        set(output_name "${ARG_OUTPUT_NAME}")
    else()
        set(output_name "${target}")
    endif()

    if (ARG_SHELL_FILE)
        set(shell_file "${ARG_SHELL_FILE}")
    else()
        set(shell_file "${STUDIOSLAB_WASM_DEFAULT_SHELL_FILE}")
    endif()

    set(common_link_options
        "-sWASM=1"
        "-sUSE_WEBGL2=1"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sASSERTIONS=1"
        "-sNO_EXIT_RUNTIME=1"
        "-sENVIRONMENT=web")

    if (NOT ARG_PRELOAD_FILES)
        list(APPEND common_link_options "-sNO_FILESYSTEM=1")
    endif()

    set_target_properties(${target} PROPERTIES
        OUTPUT_NAME "${output_name}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Build/bin"
        SUFFIX ".html")

    target_compile_features(${target} PRIVATE cxx_std_23)

    if (ARG_INCLUDE_DIRECTORIES)
        target_include_directories(${target} PRIVATE ${ARG_INCLUDE_DIRECTORIES})
    endif()

    if (ARG_LIBRARIES)
        target_link_libraries(${target} PRIVATE ${ARG_LIBRARIES})
    endif()

    if (ARG_COMPILE_DEFINITIONS)
        target_compile_definitions(${target} PRIVATE ${ARG_COMPILE_DEFINITIONS})
    endif()

    target_link_options(${target} PRIVATE
        "--shell-file"
        "${shell_file}"
        ${common_link_options}
        ${ARG_LINK_OPTIONS})

    foreach(preload_file IN LISTS ARG_PRELOAD_FILES)
        target_link_options(${target} PRIVATE "--preload-file" "${preload_file}")
    endforeach()
endfunction()

function(studioslab_add_imgui_wasm_app target)
    if (NOT EMSCRIPTEN)
        message(FATAL_ERROR
            "studioslab_add_imgui_wasm_app(): target '${target}' requires the Emscripten toolchain.")
    endif()

    set(options)
    set(oneValueArgs OUTPUT_NAME SHELL_FILE)
    set(multiValueArgs
        SOURCES
        COMPILE_DEFINITIONS
        INCLUDE_DIRECTORIES
        LIBRARIES
        LINK_OPTIONS
        PRELOAD_FILES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${target}
        ${ARG_SOURCES}
        "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/imgui.cpp"
        "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/imgui_draw.cpp"
        "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/imgui_tables.cpp"
        "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/imgui_widgets.cpp"
        "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/imgui_demo.cpp"
        "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/backends/imgui_impl_glfw.cpp"
        "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/backends/imgui_impl_opengl3.cpp")

    set(imgui_compile_definitions
        "IMGUI_IMPL_OPENGL_ES3"
        "IMGUI_DEFINE_MATH_OPERATORS")

    if (NOT ARG_PRELOAD_FILES)
        list(APPEND imgui_compile_definitions "IMGUI_DISABLE_FILE_FUNCTIONS")
    endif()

    studioslab_configure_wasm_target(${target}
        OUTPUT_NAME "${ARG_OUTPUT_NAME}"
        SHELL_FILE "${ARG_SHELL_FILE}"
        COMPILE_DEFINITIONS
            ${imgui_compile_definitions}
            ${ARG_COMPILE_DEFINITIONS}
        INCLUDE_DIRECTORIES
            "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui"
            "${STUDIOSLAB_WASM_REPO_ROOT}/Lib/3rdParty/imgui/backends"
            ${ARG_INCLUDE_DIRECTORIES}
        LIBRARIES
            ${ARG_LIBRARIES}
        LINK_OPTIONS
            "-sUSE_GLFW=3"
            ${ARG_LINK_OPTIONS}
        PRELOAD_FILES
            ${ARG_PRELOAD_FILES})
endfunction()
