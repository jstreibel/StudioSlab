cmake_minimum_required(VERSION 3.19)
project(MarylouStrings)

set(CMAKE_CXX_STANDARD 20)

set(LIBDIR ../../../Lib)
include_directories($LIBDIR)

set(EXTERNAL_SRC
		${LIBDIR}/Base/Controller/Nuklear/NuklearImplementation.cpp
		${LIBDIR}/Base/Graphics/SFML/Graph.cpp)

add_executable(MarylouStrings
        Integrator.cpp
		main.cpp
		${EXTERNAL_SRC} )

target_link_libraries(MarylouStrings
		Slab-Utils
		Slab-Base

		sfml-window
		sfml-graphics
		sfml-system

		GLEW
		OpenGL
		GL


		)


message(NOTICE "Added executable MarylouStrings")