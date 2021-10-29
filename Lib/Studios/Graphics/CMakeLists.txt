set( GRAPHICS_SOURCES
		animation.cpp
		#FunctionRenderer.cpp
		graph.cpp
		#GraphCollections.cpp
		PlottingUtils.cpp
		Window.cpp
		ZoomPanRotate.cpp
		)

add_library(${STUDIOS_LIB_BASE_NAME}-Graphics STATIC ${GRAPHICS_SOURCES})