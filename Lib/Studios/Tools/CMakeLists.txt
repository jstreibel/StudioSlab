set ( TOOLS_SOURCE
		BenchmarkHistogram.cpp
		)

add_library(${STUDIOS_LIB_BASE_NAME}-Tools STATIC ${TOOLS_SOURCE})

target_link_libraries(${STUDIOS_LIB_BASE_NAME}-Tools Slab-Common)
