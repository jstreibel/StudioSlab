add_executable(Studios
        main.cpp
        ../Common/VisualHost.cpp
)

target_link_libraries(Studios Slab)

message(NOTICE "Added executable Studios")
