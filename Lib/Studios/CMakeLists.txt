cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/bin)

include_directories(./)

set ( LIB_BACKEND_SOURCES
		Backend/Backend.cpp
		Backend/Console/ConsoleBackend.cpp
		Backend/Graphic/GLUTBackend.cpp
		)

set ( LIB_CONTROLLER_SOURCES
		Controller/Console/CLInterfaceSelector.cpp
		Controller/Console/CommandLineInterfaceManager.cpp

		Controller/Interface/CommonParameters.cpp
		Controller/Interface/ParameterBase.cpp
		Controller/Interface/Interface.cpp
		Controller/Interface/InterfaceManager.cpp
		)

set ( LIB_APP_SOURCES
		App/SafetyNet.cpp
		)

add_library(Studios STATIC ${LIB_BACKEND_SOURCES} ${LIB_CONTROLLER_SOURCES} ${LIB_APP_SOURCES})
