
set ( INTERFACE_SOURCES
		Interface/Parameter.cpp
		Interface/CommonParameters.cpp
		Interface/Interface.cpp
		Interface/InterfaceManager.cpp
		)

set ( CONSOLE_SOURCES
		Interface/InterfaceSelector.cpp
		CLArgsManager.cpp
		)

set ( NUKLEAR_SOURCES
		Nuklear/NuklearImplementation.cpp
		)

add_library(${STUDIOS_LIB_BASE_NAME}-Controller STATIC ${INTERFACE_SOURCES} ${CONSOLE_SOURCES} ${NUKLEAR_SOURCES})

target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Controller
		boost_program_options
		Slab-Common
		)