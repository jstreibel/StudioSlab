
set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Tools )

set ( CORE_TOOLS_SOURCES
		Log.cpp
		Log.h
		BenchmarkHistogram.cpp
		BenchmarkHistogram.h
		ThreadPool.cpp
		ThreadPool.h

		Resources.cpp Resources.h

)

add_library( ${LIB_LOCAL_NAME} STATIC
		${CORE_TOOLS_SOURCES}
)

message(NOTICE "Added sub-library " ${LIB_LOCAL_NAME})