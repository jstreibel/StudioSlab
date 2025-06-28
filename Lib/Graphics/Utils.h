#ifndef PLOTTINGUTILS_H
#define PLOTTINGUTILS_H

#include "Graphics/SlabGraphics.h"

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/GLUTUtils.h"
#include <Utils/Types.h>
#include "Styles/Colors.h"

namespace Slab::Graphics {

    Point2D FromSpaceToViewportCoord(const Point2D &spaceCoord, const RectR &spaceRegion, const RectI &viewport );

    Point2D FromViewportToSpaceCoord(const Point2D &viewportCoord, const RectR &spaceRegion, const RectI &viewport );

    inline Point2D PixelSizeInSpace(const RectR &region, const RectI &viewport) {
        fix dx = region.width() / (DevFloat)viewport.width();
        fix dy = region.height() / (DevFloat)viewport.height();

        return {dx, dy};
    }

    inline Point2D SpaceUnitInPixels(const RectR &region, const RectI &viewport) {
        fix n =  (DevFloat)viewport.width()/region.width();
        fix m =  (DevFloat)viewport.height()/region.height();

        return {n, m};
    }
};


#endif // PLOTTINGUTILS_H
