//
// Created by joao on 11/7/25.
//

#ifndef STUDIOSLAB_PLANESTATS_H
#define STUDIOSLAB_PLANESTATS_H

#include "../../../Lib/Graphics/Interfaces/IDrawable.h"
#include "Utils/Pointer.h"
#include "../Plane/FLittlePlane.h"

struct FPlaneStats final : Graphics::IDrawable2D {

    explicit FPlaneStats(const TPointer<FLittlePlane>& Plane, b2WorldId world)
    : Plane(Plane)
    , World(world)
    , Writer(New<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::BuiltinFonts::JuliaMono_Regular(), 36)) { }
    void Draw(const Graphics::FDraw2DParams&) override;

private:
    TPointer<FLittlePlane> Plane;
    b2WorldId World{};
    TPointer<Graphics::FWriter> Writer;
};


#endif //STUDIOSLAB_PLANESTATS_H
