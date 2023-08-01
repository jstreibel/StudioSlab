
set ( TOOLSET_SOURCES
		# Numerics/Allocator.cpp
		Toolset/Device.cpp
		Formalism/Categories.cpp
		)

set ( SPACE_SOURCES
		Space/Impl/DiscreteSpace.cpp
		Space/Impl/DiscreteSpaceCPU.cpp
		Space/Impl/DiscreteSpaceGPU.cu
		Space/Space.cpp Space/Space.h
		Space/Measure.cpp Space/Measure.h
		Space/HyperCube/AAHyperCube.cpp
		Space/HyperCube/AAHyperCube.h
		Function/AllBaseFunctions.h
		Space/Impl/PointSet.cpp
		Space/Impl/PointSet.h
		Space/Point.cpp
		Space/Point.h)


set ( EXPERIMENTAL_SOURCES
		Formalism/Category.cpp
		Formalism/Category.h

		Formalism/Graph.cpp Formalism/Graph.h
		Formalism/Set.cpp Formalism/Set.h
		Formalism/MathObject.cpp
		Formalism/MathObject.h
		Formalism/Information.cpp
		Formalism/Information.h)


set ( GRAPH_SOURCES
		Graph/Graph.cpp
		Graph/PointSetGraph.cpp
		Graph/OutputOpenGL.cpp
		)


set ( NUMERICS_SOURCES
		App.cpp
		App.h

		Numerics/VoidBuilder.cpp
		Numerics/Builder.h

		Numerics/Program/Integrator.cpp
		Numerics/Program/NumericParams.cpp

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
		DifferentialEquations/2nd-Order/GordonState.h
		DifferentialEquations/2nd-Order/GordonSystem.cpp
		DifferentialEquations/2nd-Order/GordonSystem.h

		)


set ( THERMO_SOURCES
		Thermal/Utils/RandUtils.cpp
		Thermal/Utils/ThermoUtils.cpp
		Thermal/SpinNetwork.cpp

		Thermal/Metropolis/MetropolisAlgorithm.cpp

		Thermal/Model/XYNetwork.cpp

		Thermal/IO/SingleSim/SingleSimViewController.cpp

		Thermal/IO/ManyHistories/QuenchViewController.cpp
		Thermal/IO/ManyHistories/HistoryViewController.cpp

		Thermal/IO/Tools/Graph.cpp
		Thermal/IO/Tools/GraphAndAverageCalc.cpp

		Thermal/Utils/ViewerUtils.cpp

		Function/SummableFunction.h
		Function/CommonFunctions.h
		Function/FunctionProduct.h)



set (LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Phys)

add_library(${LIB_LOCAL_NAME} STATIC
		${TOOLSET_SOURCES}
		${SPACE_SOURCES}
		${GRAPH_SOURCES}
		${EXPERIMENTAL_SOURCES}
		${NUMERICS_SOURCES}
		${DIFFEQS_SOURCES}
		${THERMO_SOURCES}
		)


target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils
		Slab-Base-Backend
		Slab-Base-Controller
		)

set_property(TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_STANDARD_REQUIRED 20)
set_property(TARGET ${LIB_LOCAL_NAME} PROPERTY CUDA_ARCHITECTURES 61)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})


# add_library( ${PHYS_BASE_LIB_NAME}-CoreThermo STATIC ${THERMO_SOURCES} )
# target_link_libraries( ${PHYS_BASE_LIB_NAME}-CoreThermo sfml-graphics sfml-window sfml-system OpenGL)