cmake_minimum_required(VERSION 3.19)
project(MarylouStrings)

set(CMAKE_CXX_STANDARD 20)

set(MY_LIBS_DIR "../../StudiosLib")
include_directories(${MY_LIBS_DIR})

set(EXTERNAL_SRC
        ${MY_LIBS_DIR}/sfml-utils/Graph.cpp)

add_executable(MarylouStrings
        Integrator.cpp              Integrator.h
        NuklearImplementation.cpp
        main.cpp
        NuklearInclude.h
        ${EXTERNAL_SRC}
        )

target_link_libraries(MarylouStrings sfml-graphics sfml-window sfml-system OpenGL)