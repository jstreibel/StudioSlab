
set( GRAPHIC_BACKEND_SOURCE
		GLUT/GLUTBackend.cpp
		VTK/VTKBackend.cpp

		Events/EventListener.cpp
		../Graphics/OpenGL/GLUTUtils.h
		../Graphics/OpenGL/GLUTUtils.cpp

		SFML-Nuklear/SFML-Nuklear-Backend.cpp
		SFML-Nuklear/SFMLListener.cpp
		GUIBackend.cpp)

set( CONSOLE_BACKEND_SOURCE
		Console/ConsoleBackend.cpp)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Backend )

add_library(${LIB_LOCAL_NAME} ${GRAPHIC_BACKEND_SOURCE} ${CONSOLE_BACKEND_SOURCE})

target_link_libraries(${LIB_LOCAL_NAME}
		Slab-3rdParty-Imgui
		Slab-Utils
		Slab-Base-Graphics

		glut
		GLEW

		GL
		OpenGL

		sfml-graphics
		sfml-window
		sfml-system

		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})
