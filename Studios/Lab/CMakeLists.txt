
if( STUDIOSLAB_CUDA_SUPPORT )
else()
endif()

add_executable( StudioSlab MainStudioSlab.cpp )
target_link_libraries( StudioSlab Slab )
message(NOTICE "Added executable StudioSlab")