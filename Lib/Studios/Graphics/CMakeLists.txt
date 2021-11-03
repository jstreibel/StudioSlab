set( GRAPHICS_SOURCES
		NotHere/OutputOpenGL.cpp

		#Tools/Graph.cpp
		#Tools/plotting-utils.cpp


		WindowManagement/Window.cpp
		WindowManagement/WindowStyles.cpp

		WindowManagement/WindowContainer/WindowPanel.cpp
		WindowManagement/WindowTree/NodeWindow.cpp
		WindowManagement/WindowTree/WindowTreeBuilder.cpp


		#FunctionRenderer.cpp
		#GraphCollections.cpp

		animation.cpp
		deprecated/graph_depr.cpp
		PlottingUtils.cpp
		ZoomPanRotate.cpp
		)

add_library(${STUDIOS_LIB_BASE_NAME}-Graphics STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Graphics
		Slab-Common
		Slab-Studios-Backend
		Slab-Studios-CoreMath
		)