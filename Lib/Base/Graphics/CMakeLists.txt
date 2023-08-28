
set( GRAPHICS_SOURCES
		Animation.cpp
		ZoomPanRotate.cpp

		Styles/StylesAndColorSchemes.cpp
		Styles/ColorMap.h
		Styles/ColorMap.cpp
        Styles/Colors.cpp

		Window/StatsDisplay.cpp
		Window/Window.cpp
		Window/WindowStyles.cpp
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
		OpenGL/Texture.h		OpenGL/Texture.cpp
		OpenGL/rougier/mat4.h	OpenGL/rougier/mat4.c
		OpenGL/rougier/shader.h	OpenGL/rougier/shader.c

)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Graphics )

add_library(${LIB_LOCAL_NAME} STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		freeimage
		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})