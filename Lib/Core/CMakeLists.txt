cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

set (STUDIOS_LIB_BASE_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Core)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/bin)

add_subdirectory( Controller )
add_subdirectory( Backend )
add_subdirectory( App )
add_subdirectory( Tools )

set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME} )

add_library(${LIB_LOCAL_NAME} STATIC Base.cpp)
target_link_libraries(${LIB_LOCAL_NAME}
		Slab-Utils

		Slab-Core-Controller
		Slab-Core-Backend
		Slab-Core-App
		Slab-Core-Tools
		)

message(NOTICE "Added library " ${LIB_LOCAL_NAME})
