add_executable(Studios
        main.cpp
        ../Common/VisualHost.cpp
        ../Common/NumericsV2TaskUtils.cpp
        ../Common/SessionLiveViewStatsV2.cpp
)

target_link_libraries(Studios Slab)

message(NOTICE "Added executable Studios")
