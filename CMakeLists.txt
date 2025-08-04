cmake_minimum_required(VERSION 3.22)

option( STUDIOSLAB_CUDA_SUPPORT "Enable CUDA support in StudioSlab libraries" ON )
option( STUDIOSLAB_OMP_SUPPORT "Enable OpenMP support in StudioSlab libraries" OFF )
option( STUDIOSLAB_SFML2_SUPPORT "Enable SFML 2 support in StudioSlab" ON )
option( STUDIOSLAB_VTK_SUPPORT "Enable VTK support in StudioSlab libraries" OFF )
option( STUDIOSLAB_USE_SINGLE_PRECISION "Use single precision instead of doubles." OFF)
option( STUDIOSLAB_FAST_COMPILE "Disable O3 etc." OFF)

project(StudioSlab CXX C )

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)



# Enable modules if supported by the compiler
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules -Wno-module-import-in-extern-c -Wno-mathematical-notation-identifier-extension")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Build/bin)


set ( OpenGL_GL_PREFERENCE GLVND )
find_package( GLEW )
find_package( GLUT )
find_package( OpenGL )

set( CMAKE_PREFIX_PATH /usr/share/cmake/Modules)

if ( STUDIOSLAB_VTK_SUPPORT )
	find_package( VTK REQUIRED )
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
		FiltersGeometry
		ImagingCore
		InteractionStyle
		RenderingCore
		RenderingUI
		RenderingAnnotation
		RenderingOpenGL2
		RenderingGL2PSOpenGL2
		RenderingContextOpenGL2
		RenderingFreeType
	)
	message(STATUS "StudioSlab VTK support is ENABLED."  )
	message(STATUS "VTK includes: ${VTK_USE_FILE}")
	message(STATUS "VTK libraries: ${VTK_LIBRARIES}")

	add_compile_definitions(USE_VTK=true) # define preprocessor macro (very useful)
else ()
	message(STATUS "StudioSlab VTK support is DISABLED."  )
endif ()


if( STUDIOSLAB_OMP_SUPPORT )
	find_package(OpenMP REQUIRED)
	if( CMAKE_CXX_COMPILER_ID STREQUAL "IntelLLVM" )
		set(OMP_FLAG "-qopenmp")
	else()
		set(OMP_FLAG "-fopenmp")
	endif ()

	set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${OMP_FLAG}")
	set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${OMP_FLAG}")

	add_compile_definitions(OMP_SUPPORT=true)

	message(STATUS "StudioSlab OpenMP support is ENABLED."  )
else()

	add_compile_definitions(OMP_SUPPORT=false)

	message(STATUS "StudioSlab OpenMP support is DISABLED."  )
endif()


if( STUDIOSLAB_CUDA_SUPPORT )
	set(CMAKE_CUDA_STANDARD       20)
	set(CMAKE_CUDA_STANDARD_REQUIRED ON)
	set(CMAKE_CUDA_EXTENSIONS      OFF)

	set(CMAKE_CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})
	set(CMAKE_CUDA_ARCHITECTURES 61 75 86 )

	set(CMAKE_CUDA_COMPILER /usr/local/cuda/bin/nvcc)

	enable_language( CUDA )

	include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

	find_package( CUDAToolkit REQUIRED )

	add_compile_definitions(USE_CUDA=true) # define preprocessor macro (very useful)

	# Verbose NVCC flags
	string(APPEND CMAKE_CUDA_FLAGS " -Xcompiler=-Wall")

	message(STATUS "StudioSlab CUDA support is ENABLED:")
	message("  --> Include directory: ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}")
	message("  --> Architecture(s): ${CMAKE_CUDA_ARCHITECTURES}"  )

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

