cmake_minimum_required(VERSION 3.22)

# "Reproduce CUDA incompatibility with latest g++"
set( REPRODUCE_CUDA_INCOMPATIBILITY_ERROR  OFF )

message( NOTICE "Compatibility test error reproduction is ${REPRODUCE_CUDA_INCOMPATIBILITY_ERROR}")

if( REPRODUCE_CUDA_INCOMPATIBILITY_ERROR )

	set(CUDA_DIR /opt/cuda)
	set( CMAKE_CUDA_COMPILER /opt/cuda/bin/nvcc )
	include_directories(/opt/cuda/include)
	set( CMAKE_CUDA_ARCHITECTURES 61 )
	enable_language( CUDA )
	find_package( CUDA REQUIRED )

	# add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

	set( COMPAT_TEST_CUDA_SRC
			some_cuda_source.cu
	)

	message(AUTHOR_WARNING "Beware of using a g++ version that CUDA supports.")


endif ()

message( NOTICE "Compatibility test CUDA source: ${COMPAT_TEST_CUDA_SRC}" )
message( NOTICE "Compatibility test CUDA links: ${CUDA_LIBRARIES}" )

find_package( SFML 2.5 COMPONENTS graphics )

set( STUDIOSLAB_LIBDIR ../../Lib)
include_directories( ${STUDIOSLAB_LIBDIR} )

add_executable( CompatibilityTest
		main.cpp

		${COMPAT_TEST_CUDA_SRC}
)
target_link_libraries ( CompatibilityTest
		${CUDA_LIBRARIES}
		sfml-graphics
)
