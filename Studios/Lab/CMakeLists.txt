
if( STUDIOSLAB_CUDA_SUPPORT )
else()
endif()

set (STUDIO_EXTERNAL_SOURCES
        ../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_PlaneWaves.cpp
        ../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_Ak2.cpp
        ../Fields/RtoR-Modes/Sim/Recipes/NumericalRecipe_wkA.cpp
        ../Fields/RtoR-Modes/Sim/Recipes/Signal_Ak2_Recipe.cpp

        ../Fields/RtoR-PureSG/InteractingFormations/input-general-oscillons.cpp
        ../Fields/RtoR-PureSG/InteractingFormations/input-sym-oscillons.cpp
        ../Fields/RtoR-PureSG/SingleFormations/input-perturbed.cpp
        ../Fields/RtoR-PureSG/SingleFormations/input-shockwave.cpp
        ../Fields/RtoR-PureSG/SingleFormations/InputSingleOscillon.cpp
        ../Fields/RtoR-PureSG/Builder.cpp

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
        ParameterGUIRenderer.cpp
        ParameterGUIRenderer.h
)
target_link_libraries( StudioSlab Slab )
message(NOTICE "Added executable StudioSlab")