

set( GRAPHIC_BACKEND_SOURCE
		GLUT/GLUTBackend.h 						GLUT/GLUTBackend.cpp
		GLFW/GLFWBackend.h						GLFW/GLFWBackend.cpp
		VTK/VTKBackend.h						VTK/VTKBackend.cpp
		SFML-Nuklear/SFML-Nuklear-Backend.h		SFML-Nuklear/SFML-Nuklear-Backend.cpp
		SFML-Nuklear/SFMLListener.cpp

		Events/GUIEventListener.h Events/GUIEventListener.cpp

		GUIBackend.h							GUIBackend.cpp
        GLFW/GLFWBackend_callbacks.cpp
		Events/KeyMap.h
)

set( CONSOLE_BACKEND_SOURCE
		Console/ConsoleBackend.cpp)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Backend )

add_library(${LIB_LOCAL_NAME} ${GRAPHIC_BACKEND_SOURCE} ${CONSOLE_BACKEND_SOURCE} )

target_link_libraries( ${LIB_LOCAL_NAME}
		Slab-3rdParty-Imgui
		Slab-3rdParty-GLFreetype
		Slab-Utils
		Slab-Core-Graphics

		glut
		GLEW

		glfw

		GL
		OpenGL

		sfml-graphics
		# sfml-window
		# sfml-system

		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})
