

set ( UTIL_SOURCES
		OMPUtils.cpp
		Printing.cpp
		Utils.cpp
		)

add_library(${STUDIOSLAB_BASE_LIB_NAME}-Common STATIC ${UTIL_SOURCES})
