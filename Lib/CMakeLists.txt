
set( STUDIOSLAB_CUDA_SUPPORT ON )

set( STUDIOSLAB_BASE_LIB_NAME Slab )

if( STUDIOSLAB_CUDA_SUPPORT )
	enable_language( CUDA )
	find_package(CUDA)

	add_compile_definitions(USE_CUDA=true) # define preprocessor macro (very useful)

	include_directories(/opt/cuda/include)

	message(STATUS "StudioSlab CUDA support is ENABLED."  )
else()
	add_compile_definitions(USE_CUDA=false)

	message(STATUS "StudioSlab CUDA support is DISABLED")
endif()

include_directories(./ ./3rdParty/imgui /usr/include/freetype2/)

# in order of dependency:
add_subdirectory(3rdParty)

add_subdirectory(Common)
add_subdirectory(Base)

add_subdirectory(Phys)
add_subdirectory(Mappings)

# add_dependencies(Fields Studios)
