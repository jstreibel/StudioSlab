cmake_minimum_required ( VERSION 3.14 FATAL_ERROR )

set (STUDIOS_LIB_BASE_NAME ${STUDIOSLAB_BASE_LIB_NAME}-Core)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY /home/joao/Developer/StudioSlab/Slab/bin)

add_subdirectory( Controller )
add_subdirectory( Backend )
add_subdirectory( App )
add_subdirectory( Tools )


option( WRAP_BASE_LIB {"Wrap all \""${STUDIOS_LIB_BASE_NAME}"-\" libs in a single \"${STUDIOS_LIB_BASE_NAME}"\"" library."} ON )

if ( WRAP_BASE_LIB )

    set ( LIB_LOCAL_NAME ${STUDIOS_LIB_BASE_NAME} )

    add_library(${LIB_LOCAL_NAME} STATIC Base.cpp)
    target_link_libraries(${LIB_LOCAL_NAME}
			Slab-Utils

    		Slab-Core-Controller
    		Slab-Core-Backend
    		Slab-Core-App
    		Slab-Graphics
			Slab-Core-Tools
    		)

    message(NOTICE "Wrapping all Core libs into single library " ${LIB_LOCAL_NAME})

endif()