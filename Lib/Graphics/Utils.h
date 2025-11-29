#ifndef PLOTTINGUTILS_H
#define PLOTTINGUTILS_H

#include "Graphics/SlabGraphics.h"

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/GLUTUtils.h"
#include <Utils/Types.h>
#include "Styles/Colors.h"

namespace Slab::Graphics {

    FPoint2D FromSpaceToViewportCoord(const FPoint2D &spaceCoord, const RectR &spaceRegion, const RectI &viewport );

    FPoint2D FromViewportToSpaceCoord(const FPoint2D &viewportCoord, const RectR &spaceRegion, const RectI &viewport );

    inline FPoint2D PixelSizeInSpace(const RectR &region, const RectI &viewport) {
        fix dx = region.GetWidth() / (DevFloat)viewport.GetWidth();
        fix dy = region.GetHeight() / (DevFloat)viewport.GetHeight();

        return {dx, dy};
    }

    inline FPoint2D SpaceUnitInPixels(const RectR &region, const RectI &viewport) {
        fix n =  (DevFloat)viewport.GetWidth()/region.GetWidth();
        fix m =  (DevFloat)viewport.GetHeight()/region.GetHeight();

        return {n, m};
    }
};


#endif // PLOTTINGUTILS_H
