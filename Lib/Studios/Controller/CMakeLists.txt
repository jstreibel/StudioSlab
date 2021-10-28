
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

add_library(Studios-Controller STATIC ${INTERFACE_SOURCES} ${CONSOLE_SOURCES})
