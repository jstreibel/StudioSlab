
set ( INTERFACE_SOURCES
        Parameter/Parameter.cpp Parameter/Parameter.h
        Parameter/CommonParameters.cpp Parameter/CommonParameters.h
		Interface/Interface.cpp				Interface/Interface.h
		Interface/InterfaceManager.cpp		Interface/InterfaceManager.h
		Interface/InterfaceOwner.cpp		Interface/InterfaceOwner.h)

set ( CONSOLE_SOURCES
		Interface/InterfaceSelector.cpp
		CommandLine/CLArgsManager.cpp
		)

set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Controller )

add_library(${LIB_LOCAL_NAME} STATIC ${INTERFACE_SOURCES} ${CONSOLE_SOURCES} )

message(NOTICE "Added sub-library " ${LIB_LOCAL_NAME})