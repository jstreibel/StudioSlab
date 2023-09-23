

set( GRAPHIC_BACKEND_SOURCE
		GLUT/GLUTBackend.h 						GLUT/GLUTBackend.cpp

		GLFW/GLFWBackend.h						GLFW/GLFWBackend.cpp
        GLFW/GLFWBackend_callbacks.cpp

		VTK/VTKBackend.h						VTK/VTKBackend.cpp

		SFML-Nuklear/SFML-Nuklear-Backend.h		SFML-Nuklear/SFML-Nuklear-Backend.cpp
		SFML-Nuklear/SFMLListener.cpp

		GUIBackend.h							GUIBackend.cpp


		Events/GUIEventListener.h 				Events/GUIEventListener.cpp
		Events/KeyMap.h


		Modules/Modules.h
		Modules/ImGuiModule.h					Modules/ImGuiModule.cpp
)

set( BACKEND_SOURCE
		Console/ConsoleBackend.cpp

		BackendManager.h		BackendManager.cpp

		Implementations.h
		Modules/Module.cpp
		Modules/Module.h
		Modules/NuklearModule.cpp
		Modules/NuklearModule.h
)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Backend )

add_library(${LIB_LOCAL_NAME} ${GRAPHIC_BACKEND_SOURCE} ${BACKEND_SOURCE} )

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
