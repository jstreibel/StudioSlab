//
// Created by joao on 11/3/25.
//

#ifndef STUDIOSLAB_IDRAWABLE_H
#define STUDIOSLAB_IDRAWABLE_H

#include "IRectProvider.h"
#include "Graphics/Backend/PlatformWindow.h"

namespace Slab::Graphics {

struct FDraw2DParams {
    const int ScreenWidth, ScreenHeight;
    const RectR Region;
};

struct IDrawable2D
{
    virtual ~IDrawable2D() = default;
    virtual void Draw(const FDraw2DParams&) = 0;
};
}

#endif //STUDIOSLAB_IDRAWABLE_H