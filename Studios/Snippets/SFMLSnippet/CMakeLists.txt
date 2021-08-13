cmake_minimum_required(VERSION 3.19)
project(SFMLSnippet)

set(CMAKE_CXX_STANDARD 20)

add_executable(main.cpp Viewer.h)

target_link_libraries(SFMLSnippet sfml-graphics sfml-window sfml-system)
