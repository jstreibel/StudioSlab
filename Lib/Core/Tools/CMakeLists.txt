
set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME}-Tools )

add_library( ${LIB_LOCAL_NAME} STATIC
		Log.cpp
		Log.h
		BenchmarkHistogram.cpp
		BenchmarkHistogram.h
)

target_link_libraries(${LIB_LOCAL_NAME}	Slab-Utils )

message(NOTICE "Added library " ${LIB_LOCAL_NAME})