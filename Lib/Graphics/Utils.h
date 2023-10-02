#ifndef PLOTTINGUTILS_H
#define PLOTTINGUTILS_H

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/GLUTUtils.h"
#include <Utils/Types.h>
#include "Styles/Colors.h"

namespace Graphics {

    Point2D FromSpaceToViewportCoord(const Point2D &spaceCoord, const RectR &spaceRegion, const RectI &viewport );

    Point2D FromViewportToSpaceCoord(const Point2D &viewportCoord, const RectR &spaceRegion, const RectI &viewport );
};


#endif // PLOTTINGUTILS_H
