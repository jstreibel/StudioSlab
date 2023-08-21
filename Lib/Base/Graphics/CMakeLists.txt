
set( GRAPHICS_SOURCES
		Animation.cpp
		ZoomPanRotate.cpp

		Styles/StylesAndColorSchemes.cpp

		Window/StatsDisplay.cpp
		Window/Window.cpp
		Window/WindowStyles.cpp
		Window/WindowContainer/WindowPanel.cpp
		Window/WindowTree/NodeWindow.cpp
		Window/WindowTree/WindowTreeBuilder.cpp

		SFML/Graph.cpp

		#FunctionRenderer.cpp
		#GraphCollections.cpp

		OpenGL/Utils.cpp
		OpenGL/GLUTUtils.cpp
        OpenGL/Texture.cpp
        OpenGL/Texture.h
		Styles/ColorMap.h
		Styles/ColorMap.cpp
        Styles/Colors.cpp
		Window/WindowContainer/WindowRow.cpp
		Window/WindowContainer/WindowRow.h
		Window/WindowContainer/WindowColumn.cpp
		Window/WindowContainer/WindowColumn.h

)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Graphics )

add_library(${LIB_LOCAL_NAME} STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		freeimage
		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})