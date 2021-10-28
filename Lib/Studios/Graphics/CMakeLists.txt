set( GRAPHICS_SOURCES
		animation.cpp
		#FunctionRenderer.cpp
		graph.cpp
		#GraphCollections.cpp
		PlottingUtils.cpp
		Window.cpp
		ZoomPanRotate.cpp
		)

add_library(Studios-Graphics STATIC ${GRAPHICS_SOURCES})