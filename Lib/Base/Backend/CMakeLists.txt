set( BACKEND_SOURCE
		Backend.cpp
		)

set( GRAPHIC_BACKEND_SOURCE
		GLUT/GLUTBackend.cpp
		GLUT/GLUTEventListener.cpp
		GLUT/GLUTUtils.h
		GLUT/GLUTUtils.cpp

		SFML-Nuklear/SFML-Nuklear-Backend.cpp
		SFML-Nuklear/SFMLListener.cpp
		)

set( CONSOLE_BACKEND_SOURCE
		Console/ConsoleBackend.cpp)

add_library(${STUDIOS_LIB_BASE_NAME}-Backend ${BACKEND_SOURCE} ${GRAPHIC_BACKEND_SOURCE} ${CONSOLE_BACKEND_SOURCE})

target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Backend
		Slab-3rdParty-Imgui
		Slab-Common

		glut

		GL
		OpenGL

		sfml-graphics
		sfml-window
		sfml-system

		)