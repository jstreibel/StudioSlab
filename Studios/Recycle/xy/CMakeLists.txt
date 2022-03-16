cmake_minimum_required(VERSION 3.19)
project(XY)


set(CMAKE_CXX_STANDARD 20)
#set(PROFILE_FLAG "-pg")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message("\nDebug build")
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${PROFILE_FLAG}")
else()
	message("\nRelease build (flag -O3)")
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${PROFILE_FLAG} -O3")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

include_directories(../Studios/Lib/StudiosLib)



add_executable(xy

		../../../Lib/Studios/Controller/Nuklear/NuklearImplementation.cpp

		../../XY/main.cpp

		BasicSim.h ../../../Lib/Studios/Controller/Nuklear/NuklearSFML.h)

target_link_libraries(xy Slab-Phys-CoreThermo Slab-Studios-Controller )

#add_custom_command(TARGET Ising
#		POST_BUILD
#		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Ising> ../DataAndPlot/Ising)