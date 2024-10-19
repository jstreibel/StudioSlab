
set(STUDIOSLAB_MATH_BASE_LIB_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Math)

set ( TOOLSET_SOURCES
		Constants.h
		Toolset/NativeFunctions.h
		FunctionOperations/FunctionOperation.cpp
		FunctionOperations/FunctionOperation.h
		Toolset/FindPeak.cpp
        Function/R2toR/Model/Operators/Integral.cpp
        Function/R2toR/Model/Operators/Integral.h
)

set ( SPACE_SOURCES
        VectorSpace/Impl/ComplexSpace.cpp 			VectorSpace/Impl/ComplexSpace.h
        VectorSpace/Impl/DiscreteSpace.cpp
        VectorSpace/Impl/DiscreteSpaceCPU.cpp
        VectorSpace/Space.cpp 						VectorSpace/Space.h
        VectorSpace/Measure.cpp						VectorSpace/Measure.h
        VectorSpace/HyperCube/AAHyperCube.cpp		VectorSpace/HyperCube/AAHyperCube.h
		Function/AllBaseFunctions.h
        VectorSpace/Impl/PointSet.cpp 				VectorSpace/Impl/PointSet.h
        Point.cpp 									Point.h
        VectorSpace/DimensionMetaData.cpp	        VectorSpace/DimensionMetaData.h
        VectorSpace/Algebra/AlgebraTests.cpp
        Numerics/Odeint/OdeintAlgebra.cpp	        Numerics/Odeint/OdeintAlgebra.h
        Function/RtoR/Model/Operators/Diff.cpp
        Function/RtoR/Model/Operators/Diff.h
)

if( STUDIOSLAB_CUDA_SUPPORT )
	set ( SPACE_SOURCES_CUDA VectorSpace/Impl/DiscreteSpaceGPU.cu)
endif ()

set ( EXPERIMENTAL_SOURCES
		Formalism/Category.cpp		Formalism/Category.h

		Formalism/Graph.cpp 		Formalism/Graph.h
		Formalism/Set.cpp 			Formalism/Set.h
		Formalism/MathObject.cpp	Formalism/MathObject.h
		Formalism/Information.cpp	Formalism/Information.h

		Formalism/Categories.h		Formalism/Categories.cpp
)


set ( NUMERICS_SOURCES
		App.cpp
		App.h

		Numerics/NumericalRecipe.cpp
		Numerics/NumericTask.cpp

		Numerics/SimConfig/NumericConfig.cpp
		Numerics/SimConfig/DeviceConfig.cpp

		Numerics/Output/OutputManager.cpp

		Numerics/Output/Plugs/Socket.cpp
		Numerics/Output/Plugs/OutputConsoleMonitor.cpp
		Numerics/Output/Plugs/HistoryKeeper.cpp
		Numerics/Output/Plugs/OutputHistoryToFile.cpp
		Numerics/Output/Plugs/OutputSnapshots.cpp

		Numerics/Output/Format/BinarySOF.cpp
		Numerics/Output/Format/CustomStringSeparatedSOF.cpp
		Numerics/Output/Format/ResolutionReductionFilter.cpp

		Numerics/Output/Util/ByteData.cpp
		Numerics/Output/Util/FieldStateOutputInterface.cpp
		Numerics/Output/Util/record-interval.cpp

        Numerics/Solver/EquationState.h
		Function/NativeFunction.h
		Data/DataManager.cpp
		Data/DataManager.h
        Numerics/Solver/Solver.cpp
        Numerics/Solver/EquationState.cpp
        Numerics/Solver/BoundaryConditions.cpp
        Numerics/Method/RungeKutta4.cpp
		Numerics/Odeint/OdeintState.cpp
		Numerics/Odeint/OdeintToSlabState.cpp
		Numerics/Odeint/OdeintToSlabState.h
)

set ( THERMO_SOURCES
		Thermal/ThermoUtils.cpp
		Thermal/SpinNetwork.cpp

		Thermal/MetropolisAlgorithm.cpp

		Thermal/XYNetwork.cpp

		Function/SummableFunction.h
		Function/CommonFunctions.h
		Function/FunctionProduct.h
        Numerics/Odeint/OdeintSignatureHelper.h
)

file(GLOB_RECURSE MAPS_SOURCES "./Function/*.cpp")

add_library(${STUDIOSLAB_MATH_BASE_LIB_NAME} STATIC
		${MAPS_SOURCES} #from subdir
		${TOOLSET_SOURCES}
		${SPACE_SOURCES}
		${SPACE_SOURCES_CUDA}
		${EXPERIMENTAL_SOURCES}
		${NUMERICS_SOURCES}
		${DIFFEQS_SOURCES}
		${THERMO_SOURCES}
		Function/RtoR/Model/FunctionsCollection/NativeFunction.cpp
		Function/RtoR/Operations/FromR2toRNumeric.cpp
		Function/RtoR/Operations/FromR2toRNumeric.h
		Function/RtoR/Model/FunctionsCollection/NonlinearKGPotential.cpp
		Function/RtoR/Model/FunctionsCollection/NonlinearKGPotential.h
        Function/R2toR/Model/FunctionsCollection/AnalyticOscillon1p1_FourierImpl.cpp
        Function/R2toR/Model/FunctionsCollection/AnalyticOscillon1p1_FourierImpl.h
		MathModule.cpp
		MathModule.h
		SlabMath.cpp
		SlabMath.h
        GiNaC/GiNaCModule.cpp
        GiNaC/GiNaCModule.h
		Data/DataAllocator.h
		Data/Data.cpp
		Data/Data.h
)


target_link_libraries(${STUDIOSLAB_MATH_BASE_LIB_NAME}
		Slab-Core
		fftw3
		boost_random
		)

set_property(TARGET ${STUDIOSLAB_MATH_BASE_LIB_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20)
#set_property(TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61)

message(NOTICE "Added library " ${STUDIOSLAB_MATH_BASE_LIB_NAME} )


# add_library( ${PHYS_BASE_LIB_NAME}-CoreThermo STATIC ${THERMO_SOURCES} )
# target_link_libraries( ${PHYS_BASE_LIB_NAME}-CoreThermo sfml-graphics sfml-window sfml-system OpenGL)