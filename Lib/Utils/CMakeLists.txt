

# set ( UTIL_INTERFACES Maps.ixx Test.ixx)

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
		StringFormatting.h  StringFormatting.cpp
		Numbers.h
		Result.h
		Arrays.h			Arrays.cpp
		Files.h				Files.cpp

		Threads.cpp			Threads.h

		Arrays.cpp
		Sets.cpp			Sets.h
		Threads.cpp			Threads.h

)

if( STUDIOSLAB_CUDA_SUPPORT )
	set(UTIL_SOURCES ${UTIL_SOURCES}
			CUDAUtils.h			CUDAUtils.cpp
            List.h
            CyclicIterator.h
            TypesGPU.h
            Zip.h
            Map.h
            Compiler.h
            Map.cpp
            StringFormatting.cpp
            StringFormatting.h
    )
endif ()

set ( UTILS_LIB ${STUDIOSLAB_BASE_LIB_NAME}-Utils )

add_library(${UTILS_LIB} STATIC ${UTIL_SOURCES} ${UTIL_INTERFACES} )
if ( STUDIOSLAB_OMP_SUPPORT )
	target_link_libraries(${UTILS_LIB} omp)
endif ()
target_link_libraries(${UTILS_LIB} boost_timer )
target_sources(${UTILS_LIB} PRIVATE ${UTIL_INTERFACES})
target_compile_options(${UTILS_LIB}
		PRIVATE
		#-std=c++20
)


message(NOTICE "Added library " ${UTILS_LIB})
