set( GRAPHICS_SOURCES
		NotHere/OutputOpenGL.cpp
		animation.cpp
		#FunctionRenderer.cpp
		graph.cpp
		#GraphCollections.cpp
		PlottingUtils.cpp
		Window.cpp
		ZoomPanRotate.cpp
		)

add_library(${STUDIOS_LIB_BASE_NAME}-Graphics STATIC ${GRAPHICS_SOURCES})
target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Graphics
		Slab-Common
		Slab-Studios-Backend
		Slab-Studios-CoreMath
		)