project(StudioSlab)
cmake_minimum_required(VERSION 3.19.2)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/bin)

add_subdirectory(Apps/)
add_subdirectory(Lib/)
#add_subdirectory(xy/)

#add_dependencies(Fields-RtoR Studios Fields-Map-RtoR Fields-Core )


SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fopenmp")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")
if(CMAKE_BUILD_TYPE STREQUAL Release)
	SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3")
	SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -O3")
	message(STATUS "Using -O3 flag")
else()
	message(STATUS "Not using -O3 flag")
endif()

