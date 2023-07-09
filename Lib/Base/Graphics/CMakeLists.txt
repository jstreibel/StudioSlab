
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

		../../Phys/Graph/OutputOpenGL.cpp

		#FunctionRenderer.cpp
		#GraphCollections.cpp

		OpenGL/Utils.cpp
		../../Phys/Graph/Graph.cpp
		../../Phys/Graph/Graph.h)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Graphics )

add_library(${LIB_LOCAL_NAME} STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		freeimage
		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})