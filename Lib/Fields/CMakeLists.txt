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

		Model/FunctionsBase.h
		Model/DiscreteSpace/DiscreteSpace.cpp
		Model/DiscreteSpace/DiscreteSpaceCPU.cpp

		Maps/RtoR/Model/RtoRFunctionArbitrary.cpp
		Maps/RtoR/Model/RtoRFunctionArbitraryCPU.cpp

		Maps/R2toR/Model/R2toRFunctionArbitrary.cpp
		Maps/R2toR/Model/R2toRFunctionArbitraryCPU.cpp

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

set ( RTOR_SOURCES
		Maps/RtoR/App/SimulationsAppRtoR.cpp

		Maps/RtoR/Core/RtoRModelAllocator.cpp

		Maps/RtoR/Controller/RtoRBCInterface.cpp

		Maps/RtoR/View/OutputStructureBuilderRtoR.cpp

		Maps/RtoR/Controller/SingleFormations/input-perturbed.cpp
		Maps/RtoR/Controller/InteractingFormations/input-sym-oscillons.cpp
		Maps/RtoR/Controller/SingleFormations/input-shockwave.cpp
		Maps/RtoR/Controller/InteractingFormations/input-general-oscillons.cpp
		Maps/RtoR/Controller/ManyOscillons/InputManyOscillons.cpp
		Maps/RtoR/Controller/ManyOscillons/InputMachineGun.cpp
		Maps/RtoR/Controller/ManyOscillons/InputStatistical.cpp
		Maps/RtoR/Controller/ManyOscillons/RandUtils.cpp
		Maps/RtoR/Controller/SingleFormations/InputSingleOscillon.cpp

		Maps/RtoR/Model/FunctionsCollection/analytic-breather.cpp
		Maps/RtoR/Model/FunctionsCollection/shockwave.cpp
		Maps/RtoR/Model/FunctionsCollection/AnalyticShockwave1D.cpp
		Maps/RtoR/Model/FunctionsCollection/AnalyticOscillon.cpp
		Maps/RtoR/Model/FunctionsCollection/AbsFunction.cpp
		Maps/RtoR/Model/FunctionsCollection/AbsPerturbedFunction.cpp
		Maps/RtoR/Model/FunctionsCollection/NoiseFunction.cpp
		Maps/RtoR/Model/FunctionsCollection/QuasiShockwave.cpp
		Maps/RtoR/Model/RtoRFunctionArbitrary.cpp
		Maps/RtoR/Model/RtoRFunctionArbitraryCPU.cpp
		Maps/RtoR/Model/Derivatives/DerivativesCPU.cpp
		Maps/RtoR/Model/RtoRBoundaryCondition.cpp
		Maps/RtoR/Model/RtoRFuncArbResizable.cpp

		Maps/RtoR/View/RtoROutputHistoryToFile.cpp
		Maps/RtoR/View/Graphics/RtoROutputOpenGlShockwave.cpp
		Maps/RtoR/View/Graphics/RtoROutputOpenGL.cpp
		Maps/RtoR/View/EnergyCalculator.cpp
		Maps/RtoR/View/OutputSnapshots.cpp
		Maps/RtoR/View/Util.cpp
		)

set ( R2TOR_SOURCES
		Maps/R2toR/Controller/R2ToRInputRadialShockwave.cpp
		Maps/R2toR/Controller/R2ToRInputShockwave.cpp
		Maps/R2toR/Controller/R2ToRInputShockwaveAtT0.cpp
		Maps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.cpp
		Maps/R2toR/Model/FunctionsCollection/R2ToRRegularDelta.cpp
		Maps/R2toR/Model/Transform.cpp
		Maps/R2toR/Model/Util/Interpolator.cpp
		Maps/R2toR/Model/R2toRFunctionArbitrary.cpp
		Maps/R2toR/Model/R2toRFunctionArbitraryCPU.cpp
		Maps/R2toR/Model/R2ToRBoundaryCondition.cpp
		Maps/R2toR/View/Filters/DimensionReductionFilter.cpp
		Maps/R2toR/View/R2toROutputOpenGLShockwave.cpp
		Maps/R2toR/View/R2toROutputOpenGLGeneric.cpp
		Maps/R2toR/View/R2ToROutputOpenGlShockwaveAtT0.cpp
		Maps/R2toR/View/OutputShockwaveZeros.cpp
		Maps/R2toR/View/OutputStructureBuilderR2ToR.cpp
		Maps/R2toR/SimulationsAppR2ToR.cpp

		Maps/RtoR2/StraightLine.cpp

		Maps/R2toR/Model/Builder/R2ToRModelBuilder.cpp
		)

set( SOURCES
		${CORE_SOURCES}
		${MODEL_SOURCES}
		${VIEW_SOURCES}
		${CUDA_SOURCES} Maps/R2toR/Model/Util/EXPOSE_BASE.h)

add_library(FieldsCore ${SOURCES})
set_property(TARGET FieldsCore PROPERTY CUDA_ARCHITECTURES 61)

#add_library(FieldsCoreRtoR ${RTOR_SOURCES}})

# add_executable ( fieldsR2toR ../FieldsApps/R2toR/App/MainR2toR.cpp ${SOURCES} ${R2TOR_SOURCES} )
# target_link_libraries ( fieldsR2toR GL glut boost_program_options boost_timer ${VTK_LIBRARIES})
# set_property(TARGET fieldsR2toR PROPERTY CUDA_ARCHITECTURES 61)


#add_executable (fields-tests
#		Apps/Tests/TestsMain.cpp
#		Apps/Tests/TestsApp.cpp
#		${SOURCES}
#		Apps/Tests/OpenGLView.cpp Apps/Tests/OpenGLView.h
#		)
#target_link_libraries ( fields-tests GL glut boost_program_options boost_timer)



