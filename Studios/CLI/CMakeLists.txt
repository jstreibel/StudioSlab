add_executable(Studios
        main.cpp
        ../../Slab/Studios/Common/VisualHost.cpp
        ../../Slab/Studios/Common/NumericsV2TaskUtils.cpp
        ../../Slab/Studios/Common/V2SimulationRunners.cpp
        ../../Slab/Studios/Common/SessionLiveViewStatsV2.cpp
        ../../Slab/Studios/Common/Simulations/V2/SPISliceV2.cpp
        ../../Slab/Studios/Common/Simulations/V2/KGRtoRPlaneWavesSliceV2.cpp
        ../../Slab/Studios/Common/Monitors/V2/SPIPassiveMonitorWindowV2.cpp
        ../../Slab/Studios/Common/Monitors/V2/KGRtoRPlaneWavesPassiveMonitorWindowV2.cpp
)

target_link_libraries(Studios Slab)

message(NOTICE "Added executable Studios")
