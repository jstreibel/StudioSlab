

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
        SnapshotDBViewer/DBViewer.cpp
        SnapshotDBViewer/DatabaseParser.cpp
        SnapshotDBViewer/DatabaseParser.h
        SnapshotDBViewer/KGDispersionRelation.cpp
        SnapshotDBViewer/HistoryFileLoader.cpp
        SnapshotDBViewer/HistoryFileLoader.h
        SnapshotDBViewer/SnapshotFileLoader.cpp
        SnapshotDBViewer/SnapshotFileLoader.h
        ../Analysis/DataView.cpp
        ../Analysis/DataView.h
        ../Analysis/AnalysisMonitor.cpp
        ../Analysis/AnalysisMonitor.h
        ../Analysis/FourierAnalysis.cpp
        ../Analysis/FourierAnalysis.h
)
target_link_libraries ( RtoR-SnapshotDBViewer Slab )
message(NOTICE "Added executable RtoR-SnapshotDBViewer")