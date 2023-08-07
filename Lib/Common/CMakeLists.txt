

set ( UTIL_SOURCES
		OMPUtils.cpp
		Printing.cpp
		Utils.cpp
		Resources.cpp
		UtilsCollection/Resample.cpp

		ColorStrings.h
)

set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Utils )

add_library(${LIB_LOCAL_NAME} STATIC ${UTIL_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME} omp boost_timer )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})
