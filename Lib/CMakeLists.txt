
set( STUDIOSLAB_BASE_LIB_NAME Slab )

include_directories(./ ./3rdParty/imgui /usr/include/freetype2/)

# in order of dependency:
add_subdirectory(3rdParty)

add_subdirectory(Utils)
add_subdirectory(Core)
add_subdirectory(Math)

add_subdirectory(Models)

add_subdirectory(Graphics)

add_library(${STUDIOSLAB_BASE_LIB_NAME} STATIC StudioSlab.cpp)
target_link_libraries(${STUDIOSLAB_BASE_LIB_NAME}
        Slab-Utils
        Slab-Core
        Slab-Math
        Slab-Graphics
        Slab-Models
)
message(NOTICE "Added library " ${STUDIOSLAB_BASE_LIB_NAME})

# add_dependencies(Fields Studios)
