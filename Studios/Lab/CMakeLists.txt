
if( STUDIOSLAB_CUDA_SUPPORT )
else()
endif()

add_executable( StudioSlab MainStudioSlab.cpp
        StudioSlabApp.cpp
        StudioSlabApp.h
        StudioWindowManager.cpp
        StudioWindowManager.h
        StudioConfig.h
        Simulation/SimulationManager.cpp
        Simulation/SimulationManager.h)
target_link_libraries( StudioSlab Slab )
message(NOTICE "Added executable StudioSlab")