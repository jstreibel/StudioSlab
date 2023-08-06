cmake_minimum_required(VERSION 3.26)

option( STUDIOSLAB_CUDA_SUPPORT "Enable CUDA support in StudioSlab libraries" ON )
option( STUDIOSLAB_OMP_SUPPORT "Enable OpenMP support in StudioSlab libraries" ON )
option( STUDIOSLAB_SFML2_SUPPORT "Enable SFML 2 support in StudioSlab" ON )

option( STUDIOSLAB_USE_SINGLE_PRECISION "Use single precision instead of doubles." OFF)




project(StudioSlab CXX)

set ( CMAKE_CXX_STANDARD 20 )

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/bin)


set ( OpenGL_GL_PREFERENCE GLVND )
find_package( GLEW )
find_package( GLUT )
find_package( OpenGL )
#find_package(Boost COMPONENTS program_options)
find_package(VTK)
include(${VTK_USE_FILE})
message(STATUS "VTK includes: ${VTK_USE_FILE}")

if(CMAKE_BUILD_TYPE STREQUAL Release)
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3")
	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -O3")
	message(STATUS "Using -O3 flag")
else()
	message(STATUS "Not using -O3 flag")
endif()


if( STUDIOSLAB_OMP_SUPPORT )
	find_package(OpenMP REQUIRED)
	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fopenmp")
	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

	message(STATUS "StudioSlab OpenMP support is ENABLED."  )
else()
	message(STATUS "StudioSlab OpenMP support is DISABLED."  )
endif()


if( STUDIOSLAB_CUDA_SUPPORT )
	# set(CUDA_DIR /opt/cuda)

	#set( CMAKE_CUDA_COMPILER /opt/cuda/bin/nvcc )
	include_directories(/opt/cuda/include)

	set( CMAKE_CUDA_ARCHITECTURES 72 )

	enable_language( CUDA )
	find_package( CUDA REQUIRED )

	add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

	add_compile_definitions(USE_CUDA=true) # define preprocessor macro (very useful)
	message(STATUS "StudioSlab CUDA support is ENABLED. CUDA libraries are ${CUDA_LIBRARIES}"  )
else()
	add_compile_definitions(USE_CUDA=false)
	message(STATUS "StudioSlab CUDA support is DISABLED")
endif()


if( STUDIOSLAB_SFML2_SUPPORT )
	# set( SFML_STATIC_LIBRARIES TRUE )
	find_package( SFML 2.6 COMPONENTS graphics window system REQUIRED )

	message(STATUS "StudioSlab SFML2 support is ENABLED."  )
else()
	message(STATUS "StudioSlab SFML2 support is DISABLED."  )
endif()


if( STUDIOSLAB_USE_SINGLE_PRECISION )
	message(STATUS "StudioSlab is using FP32")
	add_compile_definitions( STUDIOSLAB_USE_FP32=true )
else ()
	message(STATUS "StudioSlab is using IEEE FP64")
    add_compile_definitions( STUDIOSLAB_USE_FP32=false )
endif ()


link_directories(Build/lib)

add_subdirectory(Lib/)
add_subdirectory(Studios/)

