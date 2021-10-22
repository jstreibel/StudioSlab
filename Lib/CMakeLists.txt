cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

include_directories(./)

set ( BACKEND_SOURCES
		Backend/Backend.cpp
		Backend/Console/ConsoleBackend.cpp
		Backend/Graphic/GLUTBackend.cpp
		Backend/Program.h
		)

set ( CONTROLLER_SOURCES
		Controller/Console/CLInterfaceSelector.cpp
		Controller/Console/CommandLineInterfaceManager.cpp Controller/Console/CLInterfaceManager.h

		Controller/Interface/CommonParameters.cpp
		Controller/Interface/ParameterBase.cpp					Controller/Interface/ParameterBase.h
		Controller/Interface/ParameterUtils.cpp					Controller/Interface/ParameterUtils.h
		Controller/Interface/Interface.cpp						Controller/Interface/Interface.h
		Controller/Interface/InterfaceManager.cpp				Controller/Interface/InterfaceManager.h
		)

set ( APP_SOURCES
		App/SafetyNet.cpp
		)

