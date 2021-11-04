
#TODO MAKE POSSIBLE TO COMPILE WITHOUT GPU ( RLY? IN 2021? )
set ( FUNCTION_SOURCES
		Function/Space/DiscreteSpace.cpp
		Function/Space/DiscreteSpaceCPU.cpp
		Function/Space/DiscreteSpaceGPU.cu
		)

set ( INTEGRATOR_SOURCES
		Numerics/Integrator.cpp
		Numerics/NumericParams.cpp

		Numerics/Output/OutputManager.cpp
		Numerics/Output/OutputStructureBuilderBase.cpp

		Numerics/Output/Channel/OutputChannel.cpp
		Numerics/Output/Channel/OutputConsoleMonitor.cpp
		Numerics/Output/Channel/OutputHistory.cpp
		Numerics/Output/Channel/OutputHistoryToFile.cpp

		Numerics/Output/Format/BinarySOF.cpp
		Numerics/Output/Format/CustomStringSeparatedSOF.cpp
		Numerics/Output/Format/ResolutionReductionFilter.cpp

		Numerics/Output/Util/ByteData.cpp
		Numerics/Output/Util/FieldStateOutputInterface.cpp
		Numerics/Output/Util/record-interval.cpp
		)

set ( BASE_SOURCES
		Allocator.cpp
		Device.cpp
		Primitives.cpp
		)


add_library(${PHYS_BASE_LIB_NAME}-CoreMath STATIC ${FUNCTION_SOURCES} ${INTEGRATOR_SOURCES} ${BASE_SOURCES})

target_link_libraries(${PHYS_BASE_LIB_NAME}-CoreMath
		Slab-Common
		Slab-Studios-Backend
		Slab-Studios-Controller
		)

set_property(TARGET ${PHYS_BASE_LIB_NAME}-CoreMath PROPERTY CUDA_ARCHITECTURES 61)

