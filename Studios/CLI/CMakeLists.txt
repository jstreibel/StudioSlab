add_executable(Studios
        main.cpp
        ../Common/VisualHost.cpp
        ../Common/NumericsV2TaskUtils.cpp
        ../Common/V2SimulationRunners.cpp
        ../Common/SessionLiveViewStatsV2.cpp
        ../Common/Monitors/V2/SPIPassiveMonitorWindowV2.cpp
        ../Common/Monitors/V2/KGRtoRPlaneWavesPassiveMonitorWindowV2.cpp
)

target_link_libraries(Studios Slab)

message(NOTICE "Added executable Studios")
