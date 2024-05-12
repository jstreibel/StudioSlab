

set ( UTIL_SOURCES
		OMPUtils.cpp		OMPUtils.h
		Printing.cpp		Printing.h
		Utils.cpp			Utils.h
		RandUtils.cpp		RandUtils.h
		Exception.h			Exception.cpp

		UtilsCollection/Resample.cpp	UtilsCollection/Resample.h

		Types.cpp			Types.h
		EncodingUtils.h 	EncodingUtils.cpp
        PythonUtils.h       PythonUtils.cpp
		String.h			String.cpp
		Numbers.h
		Arrays.h			Arrays.cpp
		Files.h				Files.cpp

)

if( STUDIOSLAB_CUDA_SUPPORT )
	set(UTIL_SOURCES ${UTIL_SOURCES}
			CUDAUtils.h			CUDAUtils.cpp
            Arrays.cpp
            Sets.cpp
            Sets.h
    )
endif ()

set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Utils )



add_library(${LIB_LOCAL_NAME} STATIC ${UTIL_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME} omp boost_timer )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})
