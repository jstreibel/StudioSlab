//
// Created by joao on 11/7/25.
//

#ifndef STUDIOSLAB_PLANESTATS_H
#define STUDIOSLAB_PLANESTATS_H

#include "Graphics/IDrawable.h"
#include "Utils/Pointer.h"
#include "../Physics/FLittlePlane.h"

struct FPlaneStats final : Graphics::IDrawable {

    explicit FPlaneStats(const TPointer<FLittlePlane>& Plane)
    : Plane(Plane)
    , Writer(New<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(3), 36)) { }
    void Draw(const Graphics::FPlatformWindow&) override;

private:
    TPointer<FLittlePlane> Plane;
    TPointer<Graphics::FWriter> Writer;
};


#endif //STUDIOSLAB_PLANESTATS_H