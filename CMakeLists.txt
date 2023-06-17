project(StudioSlab)
cmake_minimum_required(VERSION 3.19.2)

set ( CMAKE_CXX_STANDARD 20 )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/bin)

include_directories(Lib /usr/include/vtk)
link_directories(Build/lib)


option( STUDIOSLAB_CUDA_SUPPORT "Enable CUDA support in StudioSlab libraries" ON )
option( STUDIOSLAB_USE_SINGLE_PRECISION "Use single precision instead of doubles." OFF)

if( STUDIOSLAB_CUDA_SUPPORT )
	set( CMAKE_CUDA_COMPILER /opt/cuda/bin/nvcc )
	set( CMAKE_CUDA_ARCHITECTURES 61 )

	enable_language( CUDA )
	find_package(CUDA)

	add_compile_definitions(USE_CUDA=true) # define preprocessor macro (very useful)

	include_directories(/opt/cuda/include)

	message(STATUS "StudioSlab CUDA support is ENABLED."  )
else()
	add_compile_definitions(USE_CUDA=false)

	message(STATUS "StudioSlab CUDA support is DISABLED")
endif()


if( STUDIOSLAB_USE_SINGLE_PRECISION )
	message(NOTICE "-- StudioSlab is using FP32")
	add_compile_definitions( STUDIOSLAB_USE_FP32=true )
else ()
	message(NOTICE "-- StudioSlab is using IEEE FP64")
    add_compile_definitions( STUDIOSLAB_USE_FP32=false )
endif ()


add_subdirectory(Lib/)
add_subdirectory(Studios)

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

