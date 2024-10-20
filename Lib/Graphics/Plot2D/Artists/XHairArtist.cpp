//
// Created by joao on 1/10/23.
//

#include "XHairArtist.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

namespace Slab::Graphics {



    auto getMouseViewportCoord(const RectI &vpRect) -> Point2D {
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();
        fix hScreen = guiBackend.getScreenHeight();

        fix xMouseLocal = mouse.x - vpRect.xMin;
        fix yMouseLocal = hScreen - mouse.y - vpRect.yMin;

        return {(Real) xMouseLocal, (Real) yMouseLocal};
    }


    bool XHairArtist::draw(const Plot2DWindow &graph2D) {
        if(!graph2D.isMouseIn()) return true;

        auto vpRect = graph2D.getEffectiveViewport();
        auto region = graph2D.getRegion();
        auto rect = region.getRect();

        fix mouseLocal = getMouseViewportCoord(vpRect);

        auto XHairLocation = FromViewportToSpaceCoord(mouseLocal, rect, vpRect);

        auto label = graph2D.getXHairLabel(XHairLocation);

        auto currStyle = PlotThemeManager::GetCurrent();
        currStyle->ticksWriter->write(label, {(Real)mouseLocal.x+20, (Real)mouseLocal.y+20}, currStyle->graphNumbersColor);

        XHair.clear();
        XHair.addPoint({rect.xMin, XHairLocation.y});
        XHair.addPoint({rect.xMax, XHairLocation.y});
        XHair.addPoint({XHairLocation.x, rect.yMin});
        XHair.addPoint({XHairLocation.x, rect.yMax});

        return Graphics::OpenGL::Legacy::RenderPointSet(Slab::Naked(XHair), currStyle->XHairStyle);
    }

} // Graphics