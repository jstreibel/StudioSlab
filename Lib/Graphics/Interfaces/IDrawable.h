//
// Created by joao on 11/3/25.
//

#ifndef STUDIOSLAB_IDRAWABLE_H
#define STUDIOSLAB_IDRAWABLE_H

#include "IRectProvider.h"
#include "Graphics/Backend/PlatformWindow.h"
#include "Graphics/IDrawBackend2D.h"
#include "Utils/Pointer.h"

#include <utility>

namespace Slab::Graphics {

struct FDrawInput {
    const FPlatformWindow& Window;
};

struct IDrawable
{
    virtual ~IDrawable() = default;
    virtual void Draw(const FDrawInput&) = 0;
};

struct FDraw2DParams {
    const int ScreenWidth, ScreenHeight;
    const RectR Region;
    const RectI Viewport;
    const TPointer<IDrawBackend2D> Backend;

    FDraw2DParams(int screenWidth,
                  int screenHeight,
                  RectR region,
                  RectI viewport,
                  TPointer<IDrawBackend2D> backend)
    : ScreenWidth(screenWidth)
    , ScreenHeight(screenHeight)
    , Region(region)
    , Viewport(viewport)
    , Backend(std::move(backend)) { }
};

struct IDrawable2D
{
    virtual ~IDrawable2D() = default;
    virtual void Draw(const FDraw2DParams&) = 0;
};



}


#endif //STUDIOSLAB_IDRAWABLE_H
