set( BACKEND_SOURCE
		Backend.cpp
		)

set( GRAPHIC_BACKEND_SOURCE
		Graphic/GLUTBackend.cpp
		Graphic/GLUTEventListener.cpp
		Graphic/GLUTUtils.h Graphic/GLUTUtils.cpp)

set( CONSOLE_BACKEND_SOURCE
		Console/ConsoleBackend.cpp
		)

add_library(${STUDIOS_LIB_BASE_NAME}-Backend ${BACKEND_SOURCE} ${GRAPHIC_BACKEND_SOURCE} ${CONSOLE_BACKEND_SOURCE})

target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Backend
		GL
		glut
		Slab-Common
		)