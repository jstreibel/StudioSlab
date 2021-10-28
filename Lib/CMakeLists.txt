
set( STUDIOSLAB_CUDA_SUPPORT ON )

if( STUDIOSLAB_CUDA_SUPPORT )
	enable_language( CUDA )

	add_compile_definitions(USE_CUDA=true) # define um macro de preprocessador, very useful

	include_directories(/opt/cuda/include)
	#set ( CUDA_SOURCES
	#		Maps/RtoR/Model/Derivatives/DerivativesGPU.cu
	#		Maps/RtoR/Model/RtoRFunctionArbitraryGPU.cu
	#		Maps/RtoR/Model/FunctionsCollection/SignumFunction.cu
	#		Maps/RtoR/Model/RtoRSystemGordonGPU.cu
	#		../Studios/CoreMath/Function/Space/DiscreteSpaceGPU.cu
	#		Maps/R2toR/Model/R2ToRFunctionArbitraryGPU.cu
	#		)

	message(STATUS "CUDA is enabled.")
else()
	add_compile_definitions(USE_CUDA=false)

	message(STATUS "CUDA is disabled.")
endif()

include_directories(./)

add_subdirectory(Studios)
add_subdirectory(Fields)

# add_dependencies(Fields Studios)
