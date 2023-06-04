
find_package(OpenMP REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")


set ( UTIL_SOURCES
		OMPUtils.cpp
		Printing.cpp
		Utils.cpp
		Resources.cpp
		UtilsCollection/BenchmarkHistogram.cpp
		UtilsCollection/Resample.cpp

		)

set ( LIB_LOCAL_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Utils )

add_library(${LIB_LOCAL_NAME} STATIC ${UTIL_SOURCES})
target_link_libraries(${LIB_LOCAL_NAME} omp resample boost_timer )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})
