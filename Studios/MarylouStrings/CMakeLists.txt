cmake_minimum_required(VERSION 3.19)
project(MarylouStrings)

set(CMAKE_CXX_STANDARD 20)

set(MY_LIBS_DIR "../lib")

set(ADDITIONAL_SRC ${MY_LIBS_DIR}/sfml-utils/Graph.cpp)

add_executable(MarylouStrings Integrator.cpp NuklearImplementation.cpp main.cpp NuklearInclude.h main.cpp NuklearImplementation.cpp Integrator.h ${ADDITIONAL_SRC} )


target_link_libraries(MarylouStrings sfml-graphics sfml-window sfml-system OpenGL)