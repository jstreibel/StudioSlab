
set(STUDIOSLAB_MATH_BASE_LIB_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Math)

add_subdirectory(Function)

set ( TOOLSET_SOURCES
		Constants.h
		Toolset/NativeFunctions.h
		)

set ( SPACE_SOURCES
		Space/Impl/ComplexSpace.cpp		Space/Impl/ComplexSpace.h
		Space/Impl/DiscreteSpace.cpp
		Space/Impl/DiscreteSpaceCPU.cpp
		Space/Space.cpp 				Space/Space.h
		Space/Measure.cpp 				Space/Measure.h
		Space/HyperCube/AAHyperCube.cpp Space/HyperCube/AAHyperCube.h
		Function/AllBaseFunctions.h
		Space/Impl/PointSet.cpp			Space/Impl/PointSet.h
		Space/Point.cpp					Space/Point.h
		Space/DimensionMetaData.cpp
		Space/DimensionMetaData.h
)

if( STUDIOSLAB_CUDA_SUPPORT )
set ( SPACE_SOURCES_CUDA Space/Impl/DiscreteSpaceGPU.cu )
endif ()

set ( EXPERIMENTAL_SOURCES
		Formalism/Category.cpp
		Formalism/Category.h

		Formalism/Graph.cpp Formalism/Graph.h
		Formalism/Set.cpp Formalism/Set.h
		Formalism/MathObject.cpp
		Formalism/MathObject.h
		Formalism/Information.cpp
		Formalism/Information.h

		Formalism/Categories.h		Formalism/Categories.cpp
)


set ( NUMERICS_SOURCES
		App.cpp
		App.h

		Numerics/VoidBuilder.cpp
		Numerics/Builder.h

		Numerics/Program/Integrator.cpp
		Numerics/SimConfig/NumericConfig.cpp
		Numerics/SimConfig/DeviceConfig.cpp
		Numerics/SimConfig/SimulationConfig.cpp
		Numerics/SimConfig/SimulationConfig.h

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

		DifferentialEquations/EquationState.h
		Function/NativeFunction.h
        Numerics/Data/DataManager.cpp
        Numerics/Data/DataManager.h
		Numerics/Data/DataSet.cpp
		Numerics/Data/DataSet.h
)

set ( THERMO_SOURCES
		Thermal/ThermoUtils.cpp
		Thermal/SpinNetwork.cpp

		Thermal/MetropolisAlgorithm.cpp

		Thermal/XYNetwork.cpp

		Function/SummableFunction.h
		Function/CommonFunctions.h
		Function/FunctionProduct.h
)

add_library(${STUDIOSLAB_MATH_BASE_LIB_NAME} STATIC
		${TOOLSET_SOURCES}
		${SPACE_SOURCES}
		${SPACE_SOURCES_CUDA}
		${EXPERIMENTAL_SOURCES}
		${NUMERICS_SOURCES}
		${DIFFEQS_SOURCES}
		${THERMO_SOURCES}
		)


target_link_libraries(${STUDIOSLAB_MATH_BASE_LIB_NAME}
		Slab-Core
		${STUDIOSLAB_MATH_BASE_LIB_NAME}-Maps
		fftw3
		)

set_property(TARGET ${STUDIOSLAB_MATH_BASE_LIB_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20)
#set_property(TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61)

message(NOTICE "Added library " ${STUDIOSLAB_MATH_BASE_LIB_NAME} )


# add_library( ${PHYS_BASE_LIB_NAME}-CoreThermo STATIC ${THERMO_SOURCES} )
# target_link_libraries( ${PHYS_BASE_LIB_NAME}-CoreThermo sfml-graphics sfml-window sfml-system OpenGL)