
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


set ( RTOR_SOURCES
		FunctionRenderer.cpp

        RtoR/Model/FunctionsCollection/Oscillons/analytic-breather.cpp
		RtoR/Model/FunctionsCollection/Shockwave/shockwave.cpp
		RtoR/Model/FunctionsCollection/Shockwave/AnalyticShockwave1D.cpp
        RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.cpp
		RtoR/Model/FunctionsCollection/AbsFunction.cpp
		RtoR/Model/FunctionsCollection/AbsPerturbedFunction.cpp
		RtoR/Model/FunctionsCollection/NoiseFunction.cpp
		RtoR/Model/FunctionsCollection/Shockwave/QuasiShockwave.cpp

		RtoR/Model/RtoRDiscreteFunction.cpp
		RtoR/Model/RtoRDiscreteFunctionCPU.cpp
		RtoR/Model/Derivatives/DerivativesCPU.cpp

		RtoR/Model/RtoRResizableDiscreteFunction.cpp

		RtoR/View/RtoROutputHistoryToFile.cpp

		RtoR/View/Graphics/GraphRtoR.cpp
		RtoR/View/Graphics/RtoROutputOpenGLShockwave.cpp

		RtoR/Calc/Histogram.cpp
		RtoR/Model/FunctionsCollection/Section1D.cpp
		RtoR/Model/FunctionsCollection/Section1D.h

)


set ( R2TOR_SOURCES
		FunctionRenderer.cpp


		RtoR/Model/FunctionsCollection/AbsFunction.cpp

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

		RtoR/View/Graphics/GraphRtoR.cpp


		R2toR/View/Artists/GraphR2ToR.cpp
		R2toR/View/Artists/GraphR2Section.cpp
		R2toR/View/Filters/DimensionReductionFilter.cpp
		R2toR/View/R2toROutputOpenGLShockwave.cpp
		R2toR/View/R2toROutputOpenGLGeneric.cpp
		R2toR/View/R2ToROutputOpenGlShockwaveAtT0.cpp
		#R2toR/View/OutputShockwaveZeros.cpp
		#R2toR/View/OutputStructureBuilderR2ToR.cpp
		#R2toR/View/LastOutputVtkVisualizer.cpp	R2toR/View/LastOutputVtkVisualizer.h

		RtoR2/StraightLine.cpp

		R2toR/View/Artists/FlatFieldDisplay.cpp
		R2toR/View/Artists/FlatFieldDisplay.h
		RtoR/Model/FunctionsCollection/Section1D.cpp
		RtoR/Model/FunctionsCollection/Section1D.h

)


set ( MAPS_LINKS
		Slab-Base-Controller
		Slab-Base-Backend
		Slab-Base-App
		Slab-Base-Graphics

		Slab-Utils
		Slab-Phys
		)

# ℝ ↦ ℝ

set ( MAPS_RtoR_LIB_NAME ${FIELDS_BASE_LIB_NAME}-RtoR)

set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME} )

add_library( ${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS})

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME}_GPU )

add_library(${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES} ${MAPS_CUDA_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES})
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20 )
#set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 72 )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


set ( LIB_LOCAL_NAME ${MAPS_RtoR_LIB_NAME}-Periodic_GPU )

add_library(${LIB_LOCAL_NAME} STATIC ${RTOR_SOURCES} ${MAPS_CUDA_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES})
target_compile_definitions( ${LIB_LOCAL_NAME} PUBLIC PERIODIC_BC )
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20 )
#set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61 )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


# ℝ² ↦ ℝ

set ( MAPS_R2toR_LIB_NAME ${FIELDS_BASE_LIB_NAME}-R2toR)
set ( LIB_LOCAL_NAME ${MAPS_R2toR_LIB_NAME}_GPU )

add_library(${LIB_LOCAL_NAME} STATIC ${R2TOR_SOURCES} ${MAPS_CUDA_SOURCES})
target_link_libraries( ${LIB_LOCAL_NAME} ${MAPS_LINKS} ${CUDA_LIBRARIES} ${VTK_LIBRARIES})
set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20 )
#set_property( TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61 )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})