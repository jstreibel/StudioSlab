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
        Physics/Foils/Foil.cpp
        Physics/Foils/Foil.h
        DebugDraw.h
        Physics/Foils/NACA2412.cpp
        Physics/Foils/NACA2412.h
        App.cpp
        App.h
        Physics/FLittlePlane.cpp
        Physics/FLittlePlane.h
        Physics/FPlaneFactory.cpp
        Physics/FPlaneFactory.h
        Physics/FAtmosphericCondition.cpp
        Physics/FAtmosphericCondition.h
        Physics/Terrain.cpp
        Physics/Terrain.h
        Camera.cpp
        Camera.h
        Interfaces.h
        Render/PlaneStats.cpp
        Render/PlaneStats.h
        Render/Drawables.cpp
        Render/Drawables.h
)
target_include_directories(LittlePlaneDesigner PRIVATE ${box2d_SOURCE_DIR}/include)
# target_link_libraries( LittlePlaneDesigner Slab box2D)
target_link_libraries( LittlePlaneDesigner Slab box2d::box2d)
message(NOTICE "Added executable StudioSlab")
