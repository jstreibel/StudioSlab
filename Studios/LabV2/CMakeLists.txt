add_executable( StudioSlabV2
        MainStudioSlabV2.cpp
        StudioSlabV2App.cpp
        StudioSlabV2App.h
        LabV2WindowManager.cpp
        LabV2WindowManager.h
        StudioConfigV2.h
)
target_link_libraries( StudioSlabV2 Slab )
message(NOTICE "Added executable StudioSlabV2")
