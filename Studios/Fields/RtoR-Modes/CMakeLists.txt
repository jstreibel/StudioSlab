

add_executable ( RtoR-Modes
        Sim/Main-RtoR-Modes.cpp
        Sim/Builder.cpp
        Sim/Builder.h
        Sim/Monitor.cpp
        Sim/Monitor.h
        Sim/BoundaryCondition.cpp
        Sim/BoundaryCondition.h
)
target_link_libraries ( RtoR-Modes ${SlabLibs} Slab-Maps-RtoR_GPU )


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
)
target_link_libraries ( RtoR-SnapshotDBViewer ${SlabLibs} )


message(NOTICE "Added executable RtoR-Modes")
