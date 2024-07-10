

add_executable ( RtoR-Modes
        Sim/Main-RtoR-Modes.cpp
        Sim/NumericalRecipe.cpp
        Sim/Builder.h
        Sim/Monitor.cpp
        Sim/Monitor.h
        Sim/BoundaryCondition.cpp
        Sim/BoundaryCondition.h
        Sim/SquareWave.cpp
        Sim/SquareWave.h
)
target_link_libraries ( RtoR-Modes Slab )
message(NOTICE "Added executable RtoR-Modes")


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