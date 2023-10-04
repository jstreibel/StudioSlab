
set(FIELDS_BASE_LIB_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Maps)

include_directories (..)

if( STUDIOSLAB_CUDA_SUPPORT )
	set ( MAPS_CUDA_SOURCES
			RtoR/Model/Derivatives/DerivativesGPU.cu
			RtoR/Model/RtoRDiscreteFunctionGPU.cu

			RtoR/Model/FunctionsCollection/SignumFunction.cu

			R2toR/Model/Derivatives/DerivativesGPU.cu
			R2toR/Model/R2toRDiscreteFunctionGPU.cu

			)

	message(STATUS "Maps CUDA support is ENABLED.")
else()
	add_compile_definitions(USE_CUDA=false)

	message(STATUS "CUDA is disabled in Maps.")
endif()

set ( RTOC_SOURCES
		RtoC/FourierModes.h		RtoC/FourierModes.cpp
		RtoC/Function.h
)

set ( OTHER_MAPS_SOURCES
		${RTOC_SOURCES}

		ℝ²↦ℂ/R2toCDiscreteFunction.h	ℝ²↦ℂ/R2toCDiscreteFunction.h
)



set ( RTOR_SOURCES
		FunctionRenderer.cpp

		RtoR/Model/Derivatives/DerivativesCPU.cpp


        RtoR/Model/FunctionsCollection/Oscillons/analytic-breather.cpp
		RtoR/Model/FunctionsCollection/Shockwave/shockwave.cpp
		RtoR/Model/FunctionsCollection/Shockwave/AnalyticShockwave1D.cpp
        RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.cpp
		RtoR/Model/FunctionsCollection/AbsFunction.cpp
		RtoR/Model/FunctionsCollection/AbsPerturbedFunction.cpp
		RtoR/Model/FunctionsCollection/NoiseFunction.cpp
		RtoR/Model/FunctionsCollection/Shockwave/QuasiShockwave.cpp
		RtoR/Model/FunctionsCollection/Section1D.h							RtoR/Model/FunctionsCollection/Section1D.cpp

		RtoR/Model/RtoRDiscreteFunction.cpp
		RtoR/Model/RtoRDiscreteFunctionCPU.cpp
		RtoR/Model/RtoRResizableDiscreteFunction.cpp

		RtoR/View/RtoROutputHistoryToFile.cpp

		RtoR/Calc/Histogram.cpp
        RtoR/Calc/CorrelationCalculator.h       RtoR/Calc/CorrelationCalculator.cpp
		RtoR/Calc/FourierTransform.h			RtoR/Calc/FourierTransform.cpp
        RtoR/Model/FunctionsCollection/InverseFourier.cpp
        RtoR/Model/FunctionsCollection/InverseFourier.h
		RtoR/Model/FunctionsCollection/Trigonometric.cpp
		RtoR/Model/FunctionsCollection/Trigonometric.h
		RtoR/Model/FunctionsCollection/ComplexMagnitude.cpp
		RtoR/Model/FunctionsCollection/ComplexMagnitude.h
		RtoR/Model/FunctionsCollection/Bypass.h
)


set ( R2TOR_SOURCES
		FunctionRenderer.cpp

		R2toR/Model/R2toRDiscreteFunction.cpp
		R2toR/Model/R2toRDiscreteFunctionCPU.cpp

		R2toR/Model/Energy.cpp
		R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.cpp
		R2toR/Model/FunctionsCollection/R2ToRRegularDelta.cpp
		R2toR/Model/FunctionsCollection/GrowingHole.cpp
		R2toR/Model/Transform.cpp
		R2toR/Model/Util/Interpolator.cpp
		R2toR/Model/R2toRDiscreteFunction.cpp
		R2toR/Model/R2toRDiscreteFunctionCPU.cpp


		R2toR/Output/Filters/DimensionReductionFilter.cpp
		R2toR/Output/R2toROutputOpenGLGeneric.cpp
		#R2toR/Output/R2toROutputOpenGLShockwave.cpp
		#R2toR/Output/R2ToROutputOpenGlShockwaveAtT0.cpp
		#R2toR/View/OutputShockwaveZeros.cpp
		#R2toR/View/OutputStructureBuilderR2ToR.cpp
		#R2toR/View/LastOutputVtkVisualizer.cpp	R2toR/View/LastOutputVtkVisualizer.h

		RtoR2/StraightLine.cpp

		R2toR/Model/FunctionsCollection/CorrelationFunction/CorrelationFunction.h	R2toR/Model/FunctionsCollection/CorrelationFunction/CorrelationFunction.cpp
        R2toR/Model/FunctionsCollection/CorrelationFunction/Sampler.h		        R2toR/Model/FunctionsCollection/CorrelationFunction/Sampler.cpp
        R2toR/FourierTransform.h											        R2toR/FourierTransform.cpp

		RtoR/Model/FunctionsCollection/AbsFunction.cpp
		RtoR/Model/FunctionsCollection/Section1D.h									RtoR/Model/FunctionsCollection/Section1D.cpp
)


set ( MAPS_LINKS
		Slab-Core-Controller
		Slab-Core-Backend
		Slab-Core-App
		Slab-Core-Graphics

		Slab-Utils
		Slab-Math
		)

# ℝ ↦ ℝ

set ( MAPS_RtoR_LIB_NAME ${FIELDS_BASE_LIB_NAME}-RtoR)

set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME} )

add_library( ${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES} ${OTHER_MAPS_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS})

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME}_GPU )

add_library(${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES} ${OTHER_MAPS_SOURCES} ${MAPS_CUDA_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES})
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20 )
#set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 72 )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


# set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME}-Periodic_GPU )
#
# add_library(${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES} ${MAPS_CUDA_SOURCES})
# target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES})
# target_compile_definitions( ${LIB_LOCAL_NAME} PUBLIC PERIODIC_BC )
# set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20 )
# #set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61 )
#
# message(NOTICE "Added library " ${LIB_LOCAL_NAME})


# ℝ² ↦ ℝ

set ( MAPS_R2toR_LIB_NAME ${FIELDS_BASE_LIB_NAME}-R2toR)
set ( LIB_LOCAL_NAME ${MAPS_R2toR_LIB_NAME}_GPU )

add_library(${LIB_LOCAL_NAME} STATIC ${R2TOR_SOURCES} ${MAPS_CUDA_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES} ${VTK_LIBRARIES} fftw3)
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20 )
#set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61 )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})