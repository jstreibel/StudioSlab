
if( STUDIOSLAB_CUDA_SUPPORT )
	enable_language( CUDA )

	add_compile_definitions(USE_CUDA=true) # define um macro de preprocessador, very useful

	include_directories(/opt/cuda/include)
	set ( CUDA_SOURCES
			RtoR/Model/Derivatives/DerivativesGPU.cu
			RtoR/Model/RtoRFunctionArbitraryGPU.cu
			RtoR/Model/FunctionsCollection/SignumFunction.cu
			RtoR/Model/RtoRSystemGordonGPU.cu

			# R2toR/Model/R2ToRFunctionArbitraryGPU.cu
			)

	message(STATUS "Maps CUDA support is ENABLED")
else()
	add_compile_definitions(USE_CUDA=false)

	message(STATUS "CUDA is disabled in Maps.")
endif()


set ( COMMON_SOURCES FunctionRenderer.cpp BCInterface.cpp )


set ( RTOR_SOURCES
		${COMMON_SOURCES}

		RtoR/App/FieldsApp-RtoR.cpp

		RtoR/Core/RtoRModelAllocator.cpp

		RtoR/Controller/RtoRBCInterface.cpp
		RtoR/Controller/SingleFormations/input-perturbed.cpp
		RtoR/Controller/InteractingFormations/input-sym-oscillons.cpp
		RtoR/Controller/SingleFormations/input-shockwave.cpp
		RtoR/Controller/InteractingFormations/input-general-oscillons.cpp
		RtoR/Controller/ManyOscillons/InputManyOscillons.cpp
		RtoR/Controller/ManyOscillons/InputMachineGun.cpp
		RtoR/Controller/ManyOscillons/InputStatistical.cpp
		RtoR/Controller/ManyOscillons/RandUtils.cpp
		RtoR/Controller/SingleFormations/InputSingleOscillon.cpp

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
		RtoR/View/Graphics/RtoROutputOpenGlShockwave.cpp
		RtoR/View/Graphics/RtoROutputOpenGL.cpp
		RtoR/View/EnergyCalculator.cpp
		)


set ( R2TOR_SOURCES
		Maps/R2toR/Model/R2toRFunctionArbitrary.cpp
		Maps/R2toR/Model/R2toRFunctionArbitraryCPU.cpp

		R2toR/Controller/R2ToRInputRadialShockwave.cpp
		R2toR/Controller/R2ToRInputShockwave.cpp
		R2toR/Controller/R2ToRInputShockwaveAtT0.cpp
		R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.cpp
		R2toR/Model/FunctionsCollection/R2ToRRegularDelta.cpp
		R2toR/Model/Transform.cpp
		R2toR/Model/Util/Interpolator.cpp
		R2toR/Model/R2toRFunctionArbitrary.cpp
		R2toR/Model/R2toRFunctionArbitraryCPU.cpp
		R2toR/Model/R2ToRBoundaryCondition.cpp
		R2toR/View/Filters/DimensionReductionFilter.cpp
		R2toR/View/R2toROutputOpenGLShockwave.cpp
		R2toR/View/R2toROutputOpenGLGeneric.cpp
		R2toR/View/R2ToROutputOpenGlShockwaveAtT0.cpp
		R2toR/View/OutputShockwaveZeros.cpp
		R2toR/View/OutputStructureBuilderR2ToR.cpp
		../Mappings/R2toR/SimulationsAppR2ToR.cpp

		RtoR2/StraightLine.cpp

		R2toR/Model/Builder/R2ToRModelBuilder.cpp
		)

set ( MAPS_LINKS
		Slab-Studios-App
		Slab-Studios-Backend
		Slab-Studios-Controller
		Slab-Studios-CoreMath
		Slab-Studios-Graphics
		Slab-Studios-Tools
		)

set ( MAPS_LIB_NAME ${FIELDS_BASE_LIB_NAME}-Maps-RtoR)

add_library( ${MAPS_LIB_NAME} STATIC ${RTOR_SOURCES} )
target_link_libraries( ${MAPS_LIB_NAME} ${MAPS_LINKS})

add_library(${MAPS_LIB_NAME}_GPU STATIC ${RTOR_SOURCES} ${CUDA_SOURCES})
target_link_libraries( ${MAPS_LIB_NAME}_GPU ${MAPS_LINKS} ${CUDA_LIBRARIES})
set_property(TARGET ${MAPS_LIB_NAME}_GPU PROPERTY CUDA_ARCHITECTURES 61)

