
set(FIELDS_BASE_LIB_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Fields)

set ( OpenGL_GL_PREFERENCE GLVND )
find_package(OpenMP)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(GLUT)
find_package(OpenGL)
find_package(VTK)

include(${VTK_USE_FILE})
message(STATUS "VTK INCLUDES: ${VTK_USE_FILE}")

include_directories (..)

if( STUDIOSLAB_CUDA_SUPPORT )
	set ( CUDA_SOURCES
			RtoR/Model/Derivatives/DerivativesGPU.cu
			RtoR/Model/RtoRFunctionArbitraryGPU.cu
			RtoR/Model/RtoRSystemGordonGPU.cu

			RtoR/Model/FunctionsCollection/SignumFunction.cu

			R2toR/Model/Derivatives/DerivativesGPU.cu
			R2toR/Model/R2toRDiscreteFunctionGPU.cu

			)

	message(STATUS "Maps CUDA support is ENABLED.")
else()
	add_compile_definitions(USE_CUDA=false)

	message(STATUS "CUDA is disabled in Maps.")
endif()


set ( RTOR_SOURCES
		FunctionRenderer.cpp

		RtoR/App/FieldsApp-RtoR.cpp

		#RtoR/Core/RtoRModelAllocator.cpp
		#RtoR/Core/RtoRModelAllocator_Langevin.cpp
		#RtoR/Core/RtoRModelAllocator_Montecarlo.cpp


		RtoR/Controller/RtoRBCInterface.cpp

		RtoR/Controller/SingleFormations/input-perturbed.cpp
		RtoR/Controller/SingleFormations/input-shockwave.cpp
		RtoR/Controller/SingleFormations/InputSingleOscillon.cpp

		RtoR/Controller/InteractingFormations/input-sym-oscillons.cpp
		RtoR/Controller/InteractingFormations/input-general-oscillons.cpp

		RtoR/Controller/ThermalStudies/InputManyOscillons.cpp
		RtoR/Controller/ThermalStudies/InputMachineGun.cpp
		RtoR/Controller/ThermalStudies/InputStatistical.cpp
		RtoR/Controller/ThermalStudies/InputRandomEnergyOverDotPhi.cpp


		RtoR/Model/FunctionsCollection/analytic-breather.cpp
		RtoR/Model/FunctionsCollection/shockwave.cpp
		RtoR/Model/FunctionsCollection/AnalyticShockwave1D.cpp
		RtoR/Model/FunctionsCollection/AnalyticOscillon.cpp
		RtoR/Model/FunctionsCollection/AbsFunction.cpp
		RtoR/Model/FunctionsCollection/AbsPerturbedFunction.cpp
		RtoR/Model/FunctionsCollection/NoiseFunction.cpp
		RtoR/Model/FunctionsCollection/QuasiShockwave.cpp

		RtoR/Model/RtoRFunctionArbitrary.cpp
		RtoR/Model/RtoRFunctionArbitraryCPU.cpp
		RtoR/Model/Derivatives/DerivativesCPU.cpp
		RtoR/Model/RtoRBoundaryCondition.cpp
		RtoR/Model/RtoRFuncArbResizable.cpp


		#RtoR/View/OutputStructureBuilderRtoR.cpp
		RtoR/View/RtoROutputHistoryToFile.cpp

		RtoR/View/Graphics/Artists/GraphRtoR.cpp
		RtoR/View/Graphics/RtoROutputOpenGLShockwave.cpp
		RtoR/View/Graphics/RtoRMonitor.cpp
		RtoR/View/Graphics/RtoROutGLStatistic.cpp
		RtoR/View/Graphics/RtoROutGLStatistic.h


		RtoR/Calc/EnergyCalculator.cpp
		RtoR/Calc/Histogram.cpp

		RtoR/Model/RtoRFieldState.cpp)


set ( R2TOR_SOURCES
		FunctionRenderer.cpp


		RtoR/Model/FunctionsCollection/AbsFunction.cpp

		R2toR/Model/R2toRDiscreteFunction.cpp
		R2toR/Model/R2toRDiscreteFunctionCPU.cpp

		#R2toR/Controller/R2ToR_SimulationBuilder.cpp
		#R2toR/Controller/R2ToRInputRadialShockwave.cpp
		#R2toR/Controller/__archive__/R2ToRInputShockwave.cpp
		#R2toR/Controller/R2ToRInputShockwaveAtT0.cpp

		R2toR/Model/Energy.cpp
		R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.cpp
		R2toR/Model/FunctionsCollection/R2ToRRegularDelta.cpp
		R2toR/Model/FunctionsCollection/GrowingHole.cpp
		R2toR/Model/Transform.cpp
		R2toR/Model/Util/Interpolator.cpp
		R2toR/Model/R2toRDiscreteFunction.cpp
		R2toR/Model/R2toRDiscreteFunctionCPU.cpp
		R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.cpp

		RtoR/View/Graphics/Artists/GraphRtoR.cpp


		R2toR/View/Artists/GraphR2ToR.cpp
		R2toR/View/Artists/GraphR2Section.cpp
		R2toR/View/Filters/DimensionReductionFilter.cpp
		R2toR/View/LastOutputVtkVisualizer.cpp
		R2toR/View/R2toROutputOpenGLShockwave.cpp
		R2toR/View/R2toROutputOpenGLGeneric.cpp
		R2toR/View/R2ToROutputOpenGlShockwaveAtT0.cpp
		#R2toR/View/OutputShockwaveZeros.cpp
		#R2toR/View/OutputStructureBuilderR2ToR.cpp

		"R2toR/App/R2toR App.cpp"

		RtoR2/StraightLine.cpp

		#R2toR/Core/R2toR_Allocator.cpp
		R2toR/View/Artists/FlatFieldDisplay.cpp
		R2toR/View/Artists/FlatFieldDisplay.h

		R2toR/Controller/R2ToRSimBuilder.cpp
		R2toR/Controller/R2ToRSimBuilder.h R2toR/Model/EquationSolver.h R2toR/Model/EquationState.cpp)


set ( MAPS_LINKS
		Slab-Base-Controller
		Slab-Base-Backend
		Slab-Base-App
		Slab-Base-Graphics

		Slab-Utils
		Slab-Phys
		)

# ℝ ↦ ℝ

set ( MAPS_RtoR_LIB_NAME ${FIELDS_BASE_LIB_NAME}-Maps-RtoR)

set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME} )

add_library( ${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS})

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME}_GPU )

add_library(${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES} ${CUDA_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES})
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20)
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME}-Periodic_GPU )

add_library(${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES} ${CUDA_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES})
target_compile_definitions( ${LIB_LOCAL_NAME} PUBLIC PERIODIC_BC )
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20 )
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61 )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


# ℝ² ↦ ℝ

set ( MAPS_R2toR_LIB_NAME ${FIELDS_BASE_LIB_NAME}-Maps-R2toR)
set ( LIB_LOCAL_NAME ${MAPS_R2toR_LIB_NAME}_GPU )

add_library(${LIB_LOCAL_NAME} STATIC ${R2TOR_SOURCES} ${CUDA_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES} ${VTK_LIBRARIES})
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20 )
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61 )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})