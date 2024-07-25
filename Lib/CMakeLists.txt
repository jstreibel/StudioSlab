
set( STUDIOSLAB_BASE_LIB_NAME Slab )

include_directories(./ ./3rdParty/imgui /usr/include/freetype2/)

# in order of dependency:
add_subdirectory(3rdParty)

add_subdirectory(Utils)
add_subdirectory(Core)
add_subdirectory(Math)
add_subdirectory(Graphics)
add_subdirectory(Models)

find_library(QUADMATH_LIB quadmath)

add_library(${STUDIOSLAB_BASE_LIB_NAME} STATIC StudioSlab.cpp)
target_link_libraries(${STUDIOSLAB_BASE_LIB_NAME}
        Slab-Utils
        Slab-Core
        Slab-Math
        Slab-Graphics
        Slab-Models
)
target_compile_options(${STUDIOSLAB_BASE_LIB_NAME} PRIVATE -fmodules-ts)

if (QUADMATH_LIB)
    message(STATUS "Found quadmath library: ${QUADMATH_LIB}")
    target_compile_definitions(${STUDIOSLAB_BASE_LIB_NAME} PRIVATE HAS_QUADMATH)
    target_link_libraries(${STUDIOSLAB_BASE_LIB_NAME} ${QUADMATH_LIB})
else()
    message(status "Quadmath library NOT found. Some features may be unavailable.")
endif()

message(NOTICE "Added library " ${STUDIOSLAB_BASE_LIB_NAME})

# add_dependencies(Fields Studios)
