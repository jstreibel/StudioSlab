
set( GRAPHICS_SOURCES
		ARCHIVE/NotHere/OutputOpenGL.cpp
		ARCHIVE/deprecated/graph_depr.cpp

		Graph.cpp
		PlottingUtils.cpp


		WindowManagement/Window.cpp
		WindowManagement/WindowStyles.cpp

		WindowManagement/WindowContainer/WindowPanel.cpp
		WindowManagement/WindowTree/NodeWindow.cpp
		WindowManagement/WindowTree/WindowTreeBuilder.cpp

		#FunctionRenderer.cpp
		#GraphCollections.cpp

		animation.cpp
		PlottingUtils.cpp
		ZoomPanRotate.cpp
		)

add_library(${STUDIOS_LIB_BASE_NAME}-Graphics STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Graphics
		Slab-Common
		Slab-Studios-Backend
		#Slab-Studios-CoreMath
		)