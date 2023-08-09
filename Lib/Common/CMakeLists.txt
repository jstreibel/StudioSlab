

set ( UTIL_SOURCES
		OMPUtils.cpp	OMPUtils.h
		Printing.cpp	Printing.h
		Utils.cpp		Utils.h
		RandUtils.cpp	RandUtils.h

		Resources.cpp	Resources.h

		UtilsCollection/Resample.cpp	UtilsCollection/Resample.h

		ColorStrings.h
		CUDAUtils.cpp	CUDAUtils.h
        Types.cpp		Types.h
)

set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Utils )



add_library(${LIB_LOCAL_NAME} STATIC ${UTIL_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME} omp boost_timer )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})
