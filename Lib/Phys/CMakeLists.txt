#TODO MAKE POSSIBLE TO COMPILE WITHOUT GPU ( RLY? IN 2021? )


set (PHYS_BASE_LIB_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Phys)


set ( TOOLSET_SOURCES
		Numerics/Allocator.cpp
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
		Space/Impl/PointSet.cpp Space/Impl/PointSet.h Space/Point.cpp Space/Point.h)


set ( EXPERIMENTAL_SOURCES
		Formalism/Category.cpp
		Formalism/Category.h

		Formalism/Graph.cpp Formalism/Graph.h
		Formalism/Set.cpp Formalism/Set.h
		Formalism/MathObject.cpp
		Formalism/MathObject.h
		Formalism/Information.cpp
		Formalism/Information.h)


set ( DIFFEQS_SOURCES
		DifferentialEquations/2nd-Order/Energy.cpp
		)


set ( GRAPH_SOURCES
		Graph/PointSetGraph.cpp
		)


set ( NUMERICS_SOURCES
		Numerics/Program/Integrator.cpp
		Numerics/Program/NumericParams.cpp

		Numerics/Output/OutputManager.cpp
		Numerics/Output/StructureBuilder.cpp

		Numerics/Output/Plugs/Plug.cpp
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


add_library(${PHYS_BASE_LIB_NAME} STATIC
		${TOOLSET_SOURCES}
		${SPACE_SOURCES}
		${GRAPH_SOURCES}
		${EXPERIMENTAL_SOURCES}
		${NUMERICS_SOURCES}
		${DIFFEQS_SOURCES}
		${THERMO_SOURCES}
		)

target_link_libraries(${PHYS_BASE_LIB_NAME}
		Slab-Common
		Slab-Studios-Backend
		Slab-Studios-Controller
		)

set_property(TARGET ${PHYS_BASE_LIB_NAME} PROPERTY CUDA_ARCHITECTURES 61)


# add_library( ${PHYS_BASE_LIB_NAME}-CoreThermo STATIC ${THERMO_SOURCES} )
# target_link_libraries( ${PHYS_BASE_LIB_NAME}-CoreThermo sfml-graphics sfml-window sfml-system OpenGL)