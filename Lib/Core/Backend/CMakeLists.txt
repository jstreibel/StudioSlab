
set( BACKEND_SOURCE
		Console/ConsoleBackend.cpp
		BackendManager.h						BackendManager.cpp
		Modules/Module.h						Modules/Module.cpp
		Modules/TaskManager/TaskManager.cpp		Modules/TaskManager/TaskManager.h
)

set( GRAPHICS_BACKEND_SOURCE
		Implementations.h

		GLUT/GLUTBackend.h   					GLUT/GLUTBackend.cpp
		GLUT/GLUTListener.h 					GLUT/GLUTListener.cpp
		GLUT/GLUTEventTranslator.h  			GLUT/GLUTEventTranslator.cpp

		GLFW/GLFWBackend.h 						GLFW/GLFWBackend.cpp
		GLFW/GLFWBackend_callbacks.cpp
		GLFW/GLFWListener.h 					GLFW/GLFWListener.cpp
		GLFW/GLFWEventTranslator.h  			GLFW/GLFWEventTranslator.cpp

		SFML/SFMLBackend.h 						SFML/SFMLBackend.cpp
		SFML/SFMLEventTranslator.h 				SFML/SFMLEventTranslator.cpp
		SFML/SFMLListener.h 					SFML/SFMLListener.cpp

		VTK/VTKBackend.h 						VTK/VTKBackend.cpp

		GraphicBackend.h 						GraphicBackend.cpp

		Events/GUIEventListener.h 				Events/GUIEventListener.cpp
		Events/KeyMap.h
		Events/GUIEventTranslator.h 			Events/GUIEventTranslator.cpp

		Modules/ImGui/ImGuiModule.h				Modules/ImGui/ImGuiModule.cpp
		Modules/ImGui/ImGuiModuleGLUT.h 		Modules/ImGui/ImGuiModuleGLUT.cpp
		Modules/ImGui/ImGuiModuleGLFW.h 		Modules/ImGui/ImGuiModuleGLFW.cpp
		Modules/ImGui/ImGuiColorAndStyles.h 	Modules/ImGui/ImGuiColorAndStyles.cpp
		Modules/RealTimeAnimation.h 			Modules/RealTimeAnimation.cpp
		Modules/Nuklear/NuklearModule.h 		Modules/Nuklear/NuklearModule.cpp
		Modules/Nuklear/NuklearSFMLModule.h 	Modules/Nuklear/NuklearSFMLModule.cpp
		Modules/Nuklear/NuklearGLFWModule.h 	Modules/Nuklear/NuklearGLFWModule.cpp
		Modules/ModernOpenGLModule.h 			Modules/ModernOpenGLModule.cpp
)

set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Backend )

add_library(${LIB_LOCAL_NAME} ${BACKEND_SOURCE} ${GRAPHICS_BACKEND_SOURCE})

target_link_libraries(${LIB_LOCAL_NAME}
		Slab-3rdParty-Imgui
		Slab-3rdParty-GLFreetype

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

message(NOTICE "Added sub-library " ${LIB_LOCAL_NAME})
