cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

set (STUDIOS_LIB_BASE_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Core)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/bin)

file(GLOB_RECURSE CORE_BACKEND_SOURCES "./Backend/*.cpp")
file(GLOB_RECURSE CORE_CONTROLLER_SOURCES "./Controller/*.cpp")
file(GLOB_RECURSE CORE_TOOLS_SOURCES "./Tools/*.cpp")

add_library(${STUDIOS_LIB_BASE_NAME} STATIC
		SlabCore.cpp
		${CORE_BACKEND_SOURCES}
		${CORE_APP_SOURCES}
		${CORE_TOOLS_SOURCES}
		${CORE_CONTROLLER_SOURCES}
		SlabCore.h
		Backend/Modules/Modules.cpp
		Controller/Payload.cpp
		Controller/Payload.h
		Controller/Message.cpp
		Controller/Message.h
        Tools/UniqueObject.cpp
        Tools/UniqueObject.h
		Backend/Backend.cpp
        FObject.cpp
        FObject.h

)

target_link_libraries(${STUDIOS_LIB_BASE_NAME}
		Slab-Utils
		Slab-3rdParty-Imgui

		freeimage
		freetype-gl

		glut
		GLEW

		glfw

		GL
		OpenGL

		sfml-graphics
		# sfml-window
		# sfml-system
)

message(NOTICE "Added library " ${STUDIOS_LIB_BASE_NAME})
