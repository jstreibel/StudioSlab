


add_executable ( Blueprints-prototype
        main.cpp
        Blueprint.cpp
        Blueprint.h
        BlueprintTypes.h
        Utilities/widgets.cpp
        Utilities/drawing.cpp
        Utilities/builders.cpp
        BlueprintRenderer.cpp
        BlueprintRenderer.h
        BlueprintRenderer_Draw.cpp
)
target_link_libraries ( Blueprints-prototype ${SlabLibs} Slab)