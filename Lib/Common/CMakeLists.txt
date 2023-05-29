

set ( UTIL_SOURCES
		OMPUtils.cpp
		Printing.cpp
		Utils.cpp
		BinaryToInt.cpp
		ResourcesFolder.h STDLibInclude.cpp)

add_library(${STUDIOSLAB_BASE_LIB_NAME}-Common STATIC ${UTIL_SOURCES})
target_link_libraries(${STUDIOSLAB_BASE_LIB_NAME}-Common omp )
