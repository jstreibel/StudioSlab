
set( GRAPHICS_SOURCES
		PlottingUtils.cpp
		Animation.cpp
		PlottingUtils.cpp
		ZoomPanRotate.cpp


		Artists/StatsDisplay.cpp
		Artists/StylesAndColorSchemes.cpp

		WindowManagement/Window.cpp
		WindowManagement/WindowStyles.cpp

		WindowManagement/WindowContainer/WindowPanel.cpp
		WindowManagement/WindowTree/NodeWindow.cpp
		WindowManagement/WindowTree/WindowTreeBuilder.cpp

		ARCHIVE/NotHere/OutputOpenGL.cpp

		#FunctionRenderer.cpp
		#GraphCollections.cpp

		OpenGL/Utils.cpp Artists/Graph.cpp)

add_library(${STUDIOS_LIB_BASE_NAME}-Graphics STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Graphics
		Slab-Common
		Slab-Studios-Backend
		)