
set ( INTERFACE_SOURCES
		Interface/Parameter.cpp
		Interface/CommonParameters.cpp
		Interface/Interface.cpp
		Interface/InterfaceManager.cpp
		Interface/InterfaceOwner.cpp Interface/InterfaceOwner.h)

set ( CONSOLE_SOURCES
		Interface/InterfaceSelector.cpp
		CLArgsManager.cpp
		)

set ( NUKLEAR_SOURCES
		Nuklear/NuklearImplementation.cpp
		)

set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Controller )

add_library(${LIB_LOCAL_NAME} STATIC ${INTERFACE_SOURCES} ${CONSOLE_SOURCES} ${NUKLEAR_SOURCES})

target_link_libraries(${LIB_LOCAL_NAME} Slab-Utils )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})