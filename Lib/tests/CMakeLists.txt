# CMakeLists.txt
find_package(Catch2 3 REQUIRED)          # from v3, installs a CMake package
enable_testing()

add_executable(testsuite
        test.cpp
)
target_link_libraries(testsuite PRIVATE Catch2::Catch2WithMain)

# tell CTest about it:
include(CTest)
add_test(NAME StudioSlab-Catch2 COMMAND testsuite)

# message(STATUS "All Catch2 setup")