set ( TOOLS_SOURCE
		BenchmarkHistogram.cpp
		Resample.h
		Resample.cpp )

add_library(${STUDIOS_LIB_BASE_NAME}-Tools STATIC ${TOOLS_SOURCE})

target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Tools
		resample
		boost_timer
		Slab-Common
		)
