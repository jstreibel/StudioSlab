
set( GRAPHICS_SOURCES
		Animation.cpp
		ZoomPanRotate.cpp

		Window/StatsDisplay.cpp
		Styles/StylesAndColorSchemes.cpp

		Window/Window.cpp
		Window/WindowStyles.cpp

		Window/WindowContainer/WindowPanel.cpp
		Window/WindowTree/NodeWindow.cpp
		Window/WindowTree/WindowTreeBuilder.cpp

		../../Phys/Graph/OutputOpenGL.cpp

		#FunctionRenderer.cpp
		#GraphCollections.cpp

		OpenGL/Utils.cpp
		../../Phys/Graph/Graph.cpp ../../Phys/Graph/Graph.h)

add_library(${STUDIOS_LIB_BASE_NAME}-Graphics STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Graphics
		Slab-Common
		freeimage
		)