# CMakeLists.txt
find_package(Catch2 3 REQUIRED)          # from v3, installs a CMake package

add_executable(testsuite
        test.cpp
        test_result.cpp
        test_model_v2.cpp
        test_numerics_characterization.cpp
        test_numerics_v2.cpp
        test_plot2d_v2.cpp
        test_plot2d_legacy_reflection_bridge.cpp
        test_reflection_semantics_v1.cpp
        test_typesetting_service.cpp
        ../Utils/StringFormatting.cpp
)
target_link_libraries(testsuite PRIVATE
        Catch2::Catch2WithMain
        Slab-Math
        Slab-Graphics
        Slab-Models
        freetype
)
target_link_options(testsuite PRIVATE -Wl,--no-as-needed)

include(Catch)
if (COMMAND catch_discover_tests)
    catch_discover_tests(testsuite TEST_PREFIX "testsuite::")
else()
    add_test(NAME StudioSlab-Catch2 COMMAND testsuite)
endif()

# message(STATUS "All Catch2 setup")
