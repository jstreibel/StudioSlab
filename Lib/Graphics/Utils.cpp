//
// Created by joao on 1/10/23.
//

#include "Utils.h"


namespace Slab::Graphics {

    FPoint2D FromSpaceToViewportCoord(const FPoint2D &spaceCoord, const RectR &spaceRegion,
                                     const RectI &viewport) {
        fix x = spaceCoord.x;
        fix y = spaceCoord.y;
        fix xMin = spaceRegion.xMin;
        fix yMin = spaceRegion.yMin;
        fix Δx = spaceRegion.GetWidth();
        fix Δy = spaceRegion.GetHeight();
        fix W = viewport.GetWidth();
        fix H = viewport.GetHeight();

        return {W*(x-xMin)/Δx,
                H*(y-yMin)/Δy};
    }

    FPoint2D FromViewportToSpaceCoord(const FPoint2D &viewportCoord, const RectR &spaceRegion,
                                     const RectI &viewport) {
        fix X = viewportCoord.x;
        fix Y = viewportCoord.y;
        fix W = viewport.GetWidth();
        fix H = viewport.GetHeight();

        fix xMin = spaceRegion.xMin;
        // fix yMin = spaceRegion.yMin;
        fix yMax = spaceRegion.yMax;
        fix Δx = spaceRegion.GetWidth();
        fix Δy = spaceRegion.GetHeight();


        return {xMin + Δx*X/W, yMax - Δy*Y/H};
        // return {xMin + Δx*X/W, yMin + Δy*Y/H};
    }
};