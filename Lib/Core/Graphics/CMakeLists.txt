
set( GRAPHICS_SOURCES
		Animation.cpp
		ZoomPanRotate.cpp

		Styles/ColorMap.h 		Styles/ColorMap.cpp
		Styles/Colors.h       	Styles/Colors.cpp
		Styles/WindowStyles.h	Styles/WindowStyles.cpp

        Window/GUIWindow.cpp
		Window/Window.cpp
		Window/WindowContainer/WindowPanel.cpp
		Window/WindowTree/NodeWindow.cpp
		Window/WindowTree/WindowTreeBuilder.cpp

		Window/WindowContainer/WindowRow.cpp
		Window/WindowContainer/WindowRow.h
		Window/WindowContainer/WindowColumn.cpp
		Window/WindowContainer/WindowColumn.h

		SFML/Graph.cpp

		OpenGL/Utils.h			OpenGL/Utils.cpp
		OpenGL/GLUTUtils.h  	OpenGL/GLUTUtils.cpp
        OpenGL/Texture2D.h OpenGL/Texture2D.cpp
		OpenGL/Writer.h			OpenGL/Writer.cpp
		OpenGL/rougier/mat4.h	OpenGL/rougier/mat4.c
		OpenGL/rougier/shader.h	OpenGL/rougier/shader.c
        OpenGL/OpenGL.h
        OpenGL/GLDebug.h		OpenGL/GLDebug.cpp
		Texture1D.cpp
		Texture1D.h
)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Graphics )

add_library(${LIB_LOCAL_NAME} STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		freeimage
		freetype-gl
		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})