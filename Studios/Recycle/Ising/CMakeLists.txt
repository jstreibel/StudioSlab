cmake_minimum_required(VERSION 3.19)
project(Ising)


set(CMAKE_CXX_STANDARD 20)
#set(PROFILE_FLAG "-pg")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("\nDebug build")
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${PROFILE_FLAG}")
else()
	message("\nRelease build (flag -O3)")
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${PROFILE_FLAG} -O3")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

#include_directories()

add_executable(Ising


		COMPILE_CONFIG.h

		main.cpp

		BasicSim.h
		ManyHistories.h


		IsingExact/IsingExactCalculator.h
		IsingExact/IsingExactCalculator.cpp
		IsingExact/IsingNetwork5X5Max.cpp
		IsingExact/IsingNetwork5X5Max.h


		Utils/RandUtils.h
		Utils/RandUtils.cpp

		Utils/ViewerUtils.h
		Utils/ThermoUtils.h
		Utils/ThermoUtils.cpp


		IsingMonteCarlo/IsingMonteCarloCalculator.cpp
		IsingMonteCarlo/IsingMonteCarloCalculator.h

		IsingMonteCarlo/IsingNetwork.cpp
		IsingMonteCarlo/IsingNetwork.h


		IO/ViewControlBase.h
		IO/SingleSim/SingleSimViewController.cpp
		IO/SingleSim/SingleSimViewController.h

		IO/ManyHistories/QuenchViewController.h
		IO/ManyHistories/QuenchViewController.cpp
		IO/ManyHistories/HistoryViewController.cpp
		IO/ManyHistories/HistoryViewController.h

		IO/Tools/Graph.cpp
		IO/Tools/Graph.h
		IO/Tools/GraphAndAverageCalc.cpp
		IO/Tools/GraphAndAverageCalc.h
		IO/Tools/InputOutput.h

		ManyHistoriesManyTemperatures.h IO/Config/Palletes.h Utils/ViewerUtils.cpp)

target_link_libraries(Ising sfml-graphics sfml-window sfml-system)

#add_custom_command(TARGET Ising
#		POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Ising> ../DataAndPlot/Ising)