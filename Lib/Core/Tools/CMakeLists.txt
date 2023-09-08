
set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Tools )

set ( CORE_TOOLS_SOURCES
		Log.cpp
		Log.h
		BenchmarkHistogram.cpp
		BenchmarkHistogram.h
		Animator.cpp
		Animator.h
		ThreadPool.cpp
		ThreadPool.h
)

add_library( ${LIB_LOCAL_NAME} STATIC
		${CORE_TOOLS_SOURCES}
)

target_link_libraries(${LIB_LOCAL_NAME}	Slab-Utils )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})