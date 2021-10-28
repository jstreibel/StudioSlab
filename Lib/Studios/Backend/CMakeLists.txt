set( BACKEND_SOURCE
		Backend.cpp
		)

set( BACKEND_GRAPHIC_SOURCE
		Graphic/GLUTBackend.cpp
		Graphic/OpenGLArtistBase.cpp
		)

set( CONSOLE_BACKEND_SOURCE
		Console/ConsoleBackend.cpp
		)

add_library(Studios-Backend ${BACKEND_SOURCE} ${GRAPHIC_BACKEND_SOURCE} ${CONSOLE_BACKEND_SOURCE})