
if( STUDIOSLAB_CUDA_SUPPORT )
	enable_language( CUDA )

	add_compile_definitions(USE_CUDA=true) # define um macro de preprocessador, very useful

	include_directories(/opt/cuda/include)
	set ( CUDA_SOURCES
			RtoR/Model/Derivatives/DerivativesGPU.cu
			RtoR/Model/RtoRFunctionArbitraryGPU.cu
			RtoR/Model/RtoRSystemGordonGPU.cu

			RtoR/Model/FunctionsCollection/SignumFunction.cu

			R2toR/Model/Derivatives/DerivativesGPU.cu
			R2toR/Model/R2ToRFunctionArbitraryGPU.cu

			)

	# set_source_files_properties(${CUDA_SOURCES} PROPERTIES COMPILE_FLAGS "-DBOOST_NO_EXCEPTIONS")
	# set_source_files_properties(${CUDA_SOURCES} PROPERTIES COMPILE_FLAGS "-DBOOST_EXCEPTION_DISABLE")

	message(STATUS "Maps CUDA support is ENABLED.")
else()
	add_compile_definitions(USE_CUDA=false)

	message(STATUS "CUDA is disabled in Maps.")
endif()


set ( RTOR_SOURCES
		FunctionRenderer.cpp
		BCInterface.cpp


		RtoR/App/FieldsApp-RtoR.cpp


		RtoR/Core/RtoRModelAllocator.cpp
		RtoR/Core/RtoRModelAllocator_Langevin.cpp
		RtoR/Core/RtoRModelAllocator_Montecarlo.cpp


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


		RtoR/View/OutputStructureBuilderRtoR.cpp
		RtoR/View/RtoROutputHistoryToFile.cpp

		RtoR/View/Graphics/Artists/GraphRtoR.cpp
		RtoR/View/Graphics/RtoROutputOpenGLShockwave.cpp
		RtoR/View/Graphics/RtoROutputOpenGL.cpp
		RtoR/View/Graphics/RtoROutGLStatistic.cpp
		RtoR/View/Graphics/RtoROutGLStatistic.h


		RtoR/Calc/EnergyCalculator.cpp
		RtoR/Calc/Histogram.cpp

		)


set ( R2TOR_SOURCES
		FunctionRenderer.cpp
		BCInterface.cpp

		RtoR/Model/FunctionsCollection/AbsFunction.cpp
		RtoR/View/Graphics/Artists/GraphRtoR.cpp

		R2toR/Model/R2toRFunctionArbitrary.cpp
		R2toR/Model/R2toRFunctionArbitraryCPU.cpp

		R2toR/Controller/R2ToRBCInterface.cpp
		# R2toR/Controller/R2ToRInputRadialShockwave.cpp
		R2toR/Controller/R2ToRInputShockwave.cpp
		# R2toR/Controller/R2ToRInputShockwaveAtT0.cpp

		R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.cpp
		R2toR/Model/FunctionsCollection/R2ToRRegularDelta.cpp
		R2toR/Model/FunctionsCollection/GrowingHole.cpp
		R2toR/Model/Transform.cpp
		R2toR/Model/Util/Interpolator.cpp
		R2toR/Model/R2toRFunctionArbitrary.cpp
		R2toR/Model/R2toRFunctionArbitraryCPU.cpp
		R2toR/Model/BoundaryConditions/R2ToRBoundaryCondition.cpp

		R2toR/View/Artists/GraphR2ToR.cpp
		R2toR/View/Artists/GraphR2Section.cpp
		R2toR/View/Filters/DimensionReductionFilter.cpp
		R2toR/View/R2toROutputOpenGLShockwave.cpp
		R2toR/View/R2toROutputOpenGLGeneric.cpp
		R2toR/View/R2ToROutputOpenGlShockwaveAtT0.cpp
		# R2toR/View/OutputShockwaveZeros.cpp
		R2toR/View/OutputStructureBuilderR2ToR.cpp

		R2toR/App/SimulationsAppR2ToR.cpp

		RtoR2/StraightLine.cpp

		R2toR/Core/Allocator_R2toR.cpp
		R2toR/View/Artists/FlatFieldDisplay.cpp R2toR/View/Artists/FlatFieldDisplay.h)

set ( MAPS_LINKS
		Slab-Base-App
		Slab-Base-Backend
		Slab-Base-Controller
		Slab-Base-Graphics
		Slab-Utils
		Slab-Phys
		)

# |R |--> |R

set ( MAPS_RtoR_LIB_NAME ${FIELDS_BASE_LIB_NAME}-Maps-RtoR)

add_library( ${MAPS_RtoR_LIB_NAME} STATIC ${RTOR_SOURCES})
target_link_libraries( ${MAPS_RtoR_LIB_NAME} ${MAPS_LINKS})

add_library(${MAPS_RtoR_LIB_NAME}_GPU STATIC ${RTOR_SOURCES} ${CUDA_SOURCES})
target_link_libraries( ${MAPS_RtoR_LIB_NAME}_GPU ${MAPS_LINKS} ${CUDA_LIBRARIES})
set_property( TARGET ${MAPS_RtoR_LIB_NAME}_GPU PROPERTY CUDA_ARCHITECTURES 61)

add_library(${MAPS_RtoR_LIB_NAME}-Periodic_GPU STATIC ${RTOR_SOURCES} ${CUDA_SOURCES})
target_link_libraries( ${MAPS_RtoR_LIB_NAME}-Periodic_GPU ${MAPS_LINKS} ${CUDA_LIBRARIES})
# the line below is generating the following <command-line>: warning: ISO C++11 requires whitespace after the macro name
target_compile_definitions( ${MAPS_RtoR_LIB_NAME}-Periodic_GPU PUBLIC PERIODIC_BC )
set_property( TARGET ${MAPS_RtoR_LIB_NAME}-Periodic_GPU PROPERTY CUDA_ARCHITECTURES 61 )


# |R² |--> |R

set ( MAPS_R2toR_LIB_NAME ${FIELDS_BASE_LIB_NAME}-Maps-R2toR)


add_library(${MAPS_R2toR_LIB_NAME}_GPU STATIC ${R2TOR_SOURCES} ${CUDA_SOURCES})
target_link_libraries( ${MAPS_R2toR_LIB_NAME}_GPU ${MAPS_LINKS} ${CUDA_LIBRARIES})
set_property(TARGET ${MAPS_R2toR_LIB_NAME}_GPU PROPERTY CUDA_ARCHITECTURES 61)