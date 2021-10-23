cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

project ( CoreFields LANGUAGES CXX )

set ( CMAKE_CXX_STANDARD 17 )
set( FIELDS_CUDA_SUPPORT ON )

if( FIELDS_CUDA_SUPPORT )
	enable_language( CUDA )

	add_compile_definitions(USE_CUDA=true) # define um macro de preprocessador, very useful

	include_directories(/opt/cuda/include)
	set ( CUDA_SOURCES
			../Studios/MathTools/Maps/RtoR/Model/Derivatives/DerivativesGPU.cu
			../Studios/MathTools/Maps/RtoR/Model/RtoRFunctionArbitraryGPU.cu
			../Studios/MathTools/Maps/RtoR/Model/FunctionsCollection/SignumFunction.cu
			../Studios/MathTools/Maps/RtoR/Model/RtoRSystemGordonGPU.cu

			Model/DiscreteSpace/DiscreteSpaceGPU.cu

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
		Core/NumericalIntegration.cpp
		Core/NumericParams.cpp
		Core/Device.cpp
		)

set ( MODEL_SOURCES
		Core/Allocator.cpp

		../Studios/MathTools/FunctionsBase.h
		Model/DiscreteSpace/DiscreteSpace.cpp
		Model/DiscreteSpace/DiscreteSpaceCPU.cpp

		)

set ( VIEW_SOURCES
		View/OutputManager.cpp
		View/Util/Printing.cpp View/Util/Printing.h
		View/Util/record-interval.cpp
		View/Util/FStateOutputInterface.cpp
		View/Base/OutputHistoryToFile.cpp
		View/Base/OutputConsoleMonitor.cpp
		View/Base/OutputHistory.cpp
		View/OutputFormatter/CustomStringSeparatedSOF.cpp
		View/SpaceFilter/ResolutionReductionFilter.cpp
		View/Graphic/OutputOpenGL.cpp

		View/OutputFormatter/BinarySOF.cpp View/OutputFormatter/BinarySOF.h

		View/Util/ByteData.cpp View/Util/ByteData.h
		View/Base/OutputChannel.cpp
		)

set( SOURCES
		${CORE_SOURCES}
		${MODEL_SOURCES}
		${VIEW_SOURCES}
		${CUDA_SOURCES})

add_library(Fields-Core STATIC ${SOURCES})
# target_link_libraries (Fields GL glut boost_program_options boost_timer)
set_property(TARGET Fields-Core PROPERTY CUDA_ARCHITECTURES 61)



add_subdirectory(Maps)