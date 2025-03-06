

add_executable ( Fields-RtoR-Modes
        Sim/Main-RtoR-Modes.cpp
        Sim/Recipes/NumericalRecipe_Ak2.cpp
        Sim/Recipes/NumericalRecipe_Ak2.h
        Sim/Monitor.cpp
        Sim/Monitor.h
        Sim/BoundaryCondition.cpp
        Sim/BoundaryCondition.h
        Sim/SquareWave.cpp
        Sim/SquareWave.h
)
target_link_libraries ( Fields-RtoR-Modes Slab )
message(NOTICE "Added executable Fields-RtoR-Modes")

add_executable ( Fields-RtoR-Signal
        Sim/Main-RtoR-Modes.cpp
        Sim/Recipes/Signal_Ak2_Recipe.cpp
        Sim/Recipes/Signal_Ak2_Recipe.h
        Sim/Monitor.cpp
        Sim/Monitor.h
        Sim/BoundaryCondition.cpp
        Sim/BoundaryCondition.h
        Sim/SquareWave.cpp
        Sim/SquareWave.h
)
target_compile_definitions(Fields-RtoR-Signal    PRIVATE FIELDS_SIGNAL_Ak2)
target_link_libraries ( Fields-RtoR-Signal Slab )
message(NOTICE "Added executable Fields-RtoR-Modes")

add_executable ( Fields-RtoR-Modes-wkA
        Sim/Main-RtoR-Modes.cpp
        Sim/Recipes/NumericalRecipe_wkA.cpp
        Sim/Recipes/NumericalRecipe_wkA.h
        Sim/Monitor.cpp
        Sim/Monitor.h
        Sim/BoundaryCondition.cpp
        Sim/BoundaryCondition.h
        Sim/SquareWave.cpp
        Sim/SquareWave.h
)
target_compile_definitions(Fields-RtoR-Modes-wkA PRIVATE FIELDS_MODES_wkA)
target_link_libraries ( Fields-RtoR-Modes-wkA Slab )
message(NOTICE "Added executable Fields-RtoR-Modes-wkA")


add_executable ( RtoR-SnapshotDBViewer
        SnapshotDBViewer/main.cpp
        SnapshotDBViewer/DBViewerMulti.cpp
        SnapshotDBViewer/DBViewerSequence.cpp
        SnapshotDBViewer/DatabaseParser.cpp
        SnapshotDBViewer/DatabaseParser.h
        SnapshotDBViewer/KGDispersionRelation.cpp
        SnapshotDBViewer/SnapshotFileLoader.cpp
        SnapshotDBViewer/SnapshotFileLoader.h
        ../Viewers/HistoryFileLoader.cpp
)
target_link_libraries ( RtoR-SnapshotDBViewer Slab )
message(NOTICE "Added executable RtoR-SnapshotDBViewer")


file(GLOB_RECURSE OSCPLOT_SOURCES
        OscPlot/*
)

add_executable (osc-plot
        ${OSCPLOT_SOURCES}
)
target_link_libraries (osc-plot Slab )

message(NOTICE "Added executable osc-plot")