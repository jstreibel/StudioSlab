cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

project ( CoreFields LANGUAGES CXX )

set ( CMAKE_CXX_STANDARD 17 )
set( FIELDS_CUDA_SUPPORT ON )

if( FIELDS_CUDA_SUPPORT )
	enable_language( CUDA )

	add_compile_definitions(USE_CUDA=true) # define um macro de preprocessador, very useful

	include_directories(/opt/cuda/include)
	set ( CUDA_SOURCES
			Maps/RtoR/Model/Derivatives/DerivativesGPU.cu
			Maps/RtoR/Model/RtoRFunctionArbitraryGPU.cu
			Maps/RtoR/Model/FunctionsCollection/SignumFunction.cu
			Maps/RtoR/Model/RtoRSystemGordonGPU.cu

			../Studios/CoreMath/Function/Space/DiscreteSpaceGPU.cu

			Maps/R2toR/Model/R2ToRFunctionArbitraryGPU.cu
			)

	message(STATUS "CUDA is enabled.")
else()
	add_compile_definitions(USE_CUDA=false)

	message(STATUS "CUDA is disabled.")
endif()


set ( OpenGL_GL_PREFERENCE GLVND )
find_package(OpenMP)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(GLUT)
find_package(OpenGL)


include_directories (
		./
		../../
		#${CMAKE_SOURCE_DIR}
		#${CMAKE_CURRENT_BINARY_DIR}
		#${PWD}
)

set ( CORE_SOURCES
		../Studios/CoreMath/Numerics/Integrator.cpp
		../Studios/CoreMath/Numerics/NumericParams.cpp
		../Studios/CoreMath/Device.cpp
		)

set ( MODEL_SOURCES
		../Studios/CoreMath/Allocator.cpp

		../Studios/CoreMath/Function/FunctionsBase.h
		../Studios/CoreMath/Function/Space/DiscreteSpace.cpp
		../Studios/CoreMath/Function/Space/DiscreteSpaceCPU.cpp

		)

set ( VIEW_SOURCES
		../Studios/CoreMath/Numerics/Output/OutputManager.cpp
		../Studios/Util/Printing.cpp ../Studios/Util/Printing.h
		../Studios/CoreMath/Numerics/Output/Util/record-interval.cpp
		../Studios/CoreMath/Numerics/Output/Util/FieldStateOutputInterface.cpp
		../Studios/CoreMath/Numerics/Output/Channel/OutputHistoryToFile.cpp
		../Studios/CoreMath/Numerics/Output/Channel/OutputConsoleMonitor.cpp
		../Studios/CoreMath/Numerics/Output/Channel/OutputHistory.cpp
		../Studios/CoreMath/Numerics/Output/Format/CustomStringSeparatedSOF.cpp
		../Studios/CoreMath/Numerics/Output/Format/ResolutionReductionFilter.cpp
		../Studios/Graphics/OutputOpenGL.cpp

		../Studios/CoreMath/Numerics/Output/Format/BinarySOF.cpp ../Studios/CoreMath/Numerics/Output/Format/BinarySOF.h

		../Studios/CoreMath/Numerics/Output/Util/ByteData.cpp ../Studios/CoreMath/Numerics/Output/Util/ByteData.h
		../Studios/CoreMath/Numerics/Output/Channel/OutputChannel.cpp
		)

set( SOURCES
		${CORE_SOURCES}
		${MODEL_SOURCES}
		${VIEW_SOURCES}
		${CUDA_SOURCES})

#add_library(Fields-Core STATIC ${SOURCES})
# target_link_libraries (Fields GL glut boost_program_options boost_timer)
#set_property(TARGET Fields-Core PROPERTY CUDA_ARCHITECTURES 61)



# add_subdirectory(Maps)