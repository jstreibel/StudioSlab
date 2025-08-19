
if( STUDIOSLAB_CUDA_SUPPORT )
else()
endif()

set (STUDIO_EXTERNAL_SOURCES
        ../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_PlaneWaves.cpp
        ../Fields/RtoR-Modes/Sim/SquareWave.cpp
        ../Fields/RtoR-Modes/Sim/BoundaryCondition.cpp
        ../Fields/RtoR-Modes/Sim/Monitor.cpp
        )

add_executable( StudioSlab
        ${STUDIO_EXTERNAL_SOURCES}

        MainStudioSlab.cpp
        StudioSlabApp.cpp
        StudioSlabApp.h
        StudioWindowManager.cpp
        StudioWindowManager.h
        StudioConfig.h
        Simulation/SimulationManager.cpp
        Simulation/SimulationManager.h
        Simulation/ParameterGUIRenderer.cpp
        Simulation/ParameterGUIRenderer.h
)
target_link_libraries( StudioSlab Slab )
message(NOTICE "Added executable StudioSlab")