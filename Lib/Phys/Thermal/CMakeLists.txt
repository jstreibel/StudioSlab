
set ( THERMO_SOURCES
		Utils/RandUtils.cpp
		Utils/ThermoUtils.cpp
		SpinNetwork.cpp

		Metropolis/MetropolisAlgorithm.cpp

		Model/XYNetwork.cpp

		IO/SingleSim/SingleSimViewController.cpp

		IO/ManyHistories/QuenchViewController.cpp
		IO/ManyHistories/HistoryViewController.cpp

		IO/Tools/Graph.cpp
		IO/Tools/GraphAndAverageCalc.cpp

		Utils/ViewerUtils.cpp

		)

add_library( ${PHYS_BASE_LIB_NAME}-CoreThermo STATIC ${THERMO_SOURCES} )
target_link_libraries( ${PHYS_BASE_LIB_NAME}-CoreThermo sfml-graphics sfml-window sfml-system OpenGL)