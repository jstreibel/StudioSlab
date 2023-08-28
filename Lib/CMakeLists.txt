
set( STUDIOSLAB_BASE_LIB_NAME Slab )

include_directories(./ ./3rdParty/imgui /usr/include/freetype2/)

# in order of dependency:
add_subdirectory(3rdParty)

add_subdirectory(Common)
add_subdirectory(Core)

add_subdirectory(Math)
add_subdirectory(Mappings)

add_subdirectory(Models)

# add_dependencies(Fields Studios)
