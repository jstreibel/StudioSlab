
set( BACKEND_SOURCE
		Console/ConsoleBackend.cpp
		BackendManager.h						BackendManager.cpp
		Modules/Module.h						Modules/Module.cpp
)


set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Backend )

add_library(${LIB_LOCAL_NAME} ${BACKEND_SOURCE} )

message(NOTICE "Added sub-library " ${LIB_LOCAL_NAME})
