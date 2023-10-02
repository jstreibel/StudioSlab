//
// Created by joao on 1/10/23.
//

#include "Utils.h"


namespace Graphics {

    Point2D FromSpaceToViewportCoord(const Point2D &spaceCoord, const RectR &spaceRegion,
                                     const RectI &viewport) {
        fix x = spaceCoord.x;
        fix y = spaceCoord.y;
        fix xMin = spaceRegion.xMin;
        fix yMin = spaceRegion.yMin;
        fix Δx = spaceRegion.width();
        fix Δy = spaceRegion.height();
        fix W = viewport.width();
        fix H = viewport.height();

        return {W*(x-xMin)/Δx,
                H*(y-yMin)/Δy};
    }

    Point2D FromViewportToSpaceCoord(const Point2D &viewportCoord, const RectR &spaceRegion,
                                     const RectI &viewport) {
        fix X = viewportCoord.x;
        fix Y = viewportCoord.y;
        fix xMin = spaceRegion.xMin;
        fix yMin = spaceRegion.yMin;
        fix Δx = spaceRegion.width();
        fix Δy = spaceRegion.height();
        fix W = viewport.width();
        fix H = viewport.height();

        return {xMin + Δx*X/W, yMin + Δy*Y/H};
    }
};