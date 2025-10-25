include(FetchContent)
FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG        v3.1.1
)
FetchContent_MakeAvailable(box2d)

message(STATUS "***************************************************")
message(STATUS "${box2d_SOURCE_DIR}")
message(STATUS "***************************************************")

add_executable( LittlePlaneDesigner
        LPDMain.cpp
        Foils/Foil.cpp
        Foils/Foil.h
        DebugDraw.h
        Foils/NACA2412.cpp
        Foils/NACA2412.h
        App.cpp
        App.h
)
target_include_directories(LittlePlaneDesigner PRIVATE ${box2d_SOURCE_DIR}/include)
# target_link_libraries( LittlePlaneDesigner Slab box2D)
target_link_libraries( LittlePlaneDesigner Slab box2d::box2d)
message(NOTICE "Added executable StudioSlab")
