
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

add_library(${STUDIOSLAB_BASE_LIB_NAME}-Utils STATIC ${UTIL_SOURCES})
target_link_libraries(${STUDIOSLAB_BASE_LIB_NAME}-Utils omp resample boost_timer )
