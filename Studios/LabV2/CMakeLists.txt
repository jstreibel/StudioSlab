add_executable( StudioSlabV2
        MainStudioSlabV2.cpp
        StudioSlabV2App.cpp
        StudioSlabV2App.h
        LabV2WindowManager.cpp
        LabV2WindowManager.h
        SimulationManagerV2.cpp
        SimulationManagerV2.h
        StudioConfigV2.h

        ../../Slab/Studios/Common/VisualHost.cpp
        ../../Slab/Studios/Common/NumericsV2TaskUtils.cpp
        ../../Slab/Studios/Common/V2SimulationRunners.cpp
        ../../Slab/Studios/Common/SessionLiveViewStatsV2.cpp
        ../../Slab/Studios/Common/Simulations/V2/SPISliceV2.cpp
        ../../Slab/Studios/Common/Simulations/V2/KGRtoRPlaneWavesSliceV2.cpp
        ../../Slab/Studios/Common/Monitors/V2/SPIPassiveMonitorWindowV2.cpp
        ../../Slab/Studios/Common/Monitors/V2/KGRtoRPlaneWavesPassiveMonitorWindowV2.cpp
)
target_link_libraries( StudioSlabV2 Slab )
message(NOTICE "Added executable StudioSlabV2")
