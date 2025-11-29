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
        App.cpp
        App.h
        Interfaces.h

        Physics/Foils/Foil.cpp
        Physics/Foils/Foil.h
        Physics/Foils/NACA2412.cpp
        Physics/Foils/NACA2412.h
        Physics/FAtmosphericCondition.cpp
        Physics/FAtmosphericCondition.h
        Physics/Terrain.cpp
        Physics/Terrain.h

        Plane/FLittlePlane.cpp
        Plane/FLittlePlane.h
        Plane/FPlaneFactory.cpp
        Plane/FPlaneFactory.h

        Scenes/Camera.cpp
        Scenes/Camera.h
        Scenes/BigHill.cpp
        Scenes/BigHill.h

        Render/DebugDraw.h
        Render/PlaneStats.cpp
        Render/PlaneStats.h
        Render/Drawables.cpp
        Render/Drawables.h
        Plane/FPlaneController.cpp
        Physics/FLPDPhysicsEngine.cpp
        Scenes/FLittlePlaneBlueprint.cpp
        Scenes/FLittlePlaneBlueprint.h
        Render/DebugDraw.cpp
        Utils.cpp
        Utils.h

)
target_include_directories(LittlePlaneDesigner PRIVATE ${box2d_SOURCE_DIR}/include)
# target_link_libraries( LittlePlaneDesigner Slab box2D)
target_link_libraries( LittlePlaneDesigner Slab box2d::box2d)
message(NOTICE "Added executable StudioSlab")
