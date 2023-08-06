cmake_minimum_required(VERSION 3.26)

project( SFML_CUDA_TestProject CXX )

enable_language( CUDA )
find_package( CUDA )
find_package( SFML 2.6 COMPONENTS graphics )

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CUDA_STANDARD 26)

add_executable( SFML_CUDA_Test
		main.cpp
		some_cuda_source.cu # This line, when commented out, solves the problem. But then, no CUDA...
)
target_link_libraries ( SFML_CUDA_Test
		${CUDA_LIBRARIES}
		sfml-graphics
)
