cmake_minimum_required(VERSION 3.26)

option( STUDIOSLAB_CUDA_SUPPORT "Enable CUDA support in StudioSlab libraries" ON )
option( STUDIOSLAB_OMP_SUPPORT "Enable OpenMP support in StudioSlab libraries" ON )
option( STUDIOSLAB_SFML2_SUPPORT "Enable SFML 2 support in StudioSlab" ON )
option( STUDIOSLAB_USE_SINGLE_PRECISION "Use single precision instead of doubles." OFF)
option( STUDIOSLAB_FAST_COMPILE "Disable O3 etc." OFF)



project(StudioSlab CXX C )

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Enable modules if supported by the compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Build/bin)


set ( OpenGL_GL_PREFERENCE GLVND )
find_package( GLEW )
find_package( GLUT )
find_package( OpenGL )

set( CMAKE_PREFIX_PATH /usr/share/cmake/Modules)
find_package( VTK COMPONENTS
		CommonColor
		CommonComputationalGeometry
		CommonCore
		CommonDataModel
		CommonExecutionModel
		CommonMath
		CommonMisc
		CommonSystem
		CommonTransforms
		FiltersCore
		FiltersGeneral
		FiltersSources
		ImagingCore
		InteractionStyle
		RenderingCore
		#RenderingOpenGL
)
message(STATUS "VTK includes: ${VTK_USE_FILE}")
message(STATUS "VTK libraries: ${VTK_LIBRARIES}")

if( STUDIOSLAB_OMP_SUPPORT )
	find_package(OpenMP REQUIRED)
	if( CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" )
		set(OMP_FLAG "-qopenmp")
	else()
		set(OMP_FLAG "-fopenmp")
	endif ()

	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${OMP_FLAG}")
	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${OMP_FLAG}")

	message(STATUS "StudioSlab OpenMP support is ENABLED."  )
else()
	message(STATUS "StudioSlab OpenMP support is DISABLED."  )
endif()


if( STUDIOSLAB_CUDA_SUPPORT )
	enable_language( CUDA )

	set(CMAKE_CUDA_ARCHITECTURES 61)
	# set(CUDA_TOOLKIT_ROO /usr/local/cuda-12.5)
	set(CUDA_DIR /usr/local/cuda-12.5)
	include_directories(${CUDA_DIR}/include/)

	set( CMAKE_CUDA_COMPILER /usr/local/cuda-12.5/bin/nvcc )

	find_package( CUDA )
	find_package( CUDAToolkit )

	add_compile_definitions(USE_CUDA=true) # define preprocessor macro (very useful)
	message(STATUS "StudioSlab CUDA support is ENABLED. Architecture(s): ${CMAKE_CUDA_ARCHITECTURES}"  )
else()
	add_compile_definitions(USE_CUDA=false)
	message(STATUS "StudioSlab CUDA support is DISABLED")
endif()


if( STUDIOSLAB_SFML2_SUPPORT )
	# set( SFML_STATIC_LIBRARIES TRUE )
	find_package( SFML 2.5 COMPONENTS graphics window system REQUIRED )

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


if(STUDIOSLAB_FAST_COMPILE)
	#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O0 -Rno-debug-disables-optimization")
	#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -O0 -Rno-debug-disables-optimization")
	#message(STATUS "Using -O0 flag (for fast compile)")

elseif(CMAKE_BUILD_TYPE STREQUAL Release)
	if( CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" )
		set(IntelFlags "-fp-model strict")
	endif ()
	set(ScientificPrecisionFlags "-fno-fast-math")

	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3 ${ScientificPrecisionFlags} ${IntelFlags}")
	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -O3")
	message(STATUS "Using -O3 flag")
else()
	message(STATUS "Not using -O3 flag (CMake build type is ${CMAKE_BUILD_TYPE})")
endif()

message(STATUS "Building for ${CMAKE_SYSTEM_NAME} target system")
message(STATUS "C compiler: ${CMAKE_C_COMPILER_ID} @ ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER_ID} @ ${CMAKE_CXX_COMPILER}")
add_compile_definitions(USED_C_COMPILER="${CMAKE_C_COMPILER}") 	   # define preprocessor macro
add_compile_definitions(USED_CXX_COMPILER="${CMAKE_CXX_COMPILER}") # define preprocessor macro (very useful)

link_directories(Build/lib)

add_subdirectory(Lib/)
add_subdirectory(Studios/)

