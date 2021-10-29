
set ( INTERFACE_SOURCES
		Interface/Parameter.cpp
		Interface/CommonParameters.cpp
		Interface/Interface.cpp
		Interface/InterfaceManager.cpp
		)

set ( CONSOLE_SOURCES
		Console/CLInterfaceSelector.cpp
		Console/CLInterfaceManager.cpp
		)

add_library(${STUDIOS_LIB_BASE_NAME}-Controller STATIC ${INTERFACE_SOURCES} ${CONSOLE_SOURCES})

target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Controller Slab-Common)