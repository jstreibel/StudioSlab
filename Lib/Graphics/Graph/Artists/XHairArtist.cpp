//
// Created by joao on 1/10/23.
//

#include "XHairArtist.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/Graph/PlotThemeManager.h"
#include "Graphics/Graph/PlottingWindow.h"
#include "imgui.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

namespace Slab::Graphics {



    auto getMouseViewportCoord(const RectI &vpRect) -> Point2D {
        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        fix &mouse = guiBackend.getMouseState();
        fix hScreen = guiBackend.getScreenHeight();

        fix xMouseLocal = mouse.x - vpRect.xMin;
        fix yMouseLocal = hScreen - mouse.y - vpRect.yMin;

        return {(Real) xMouseLocal, (Real) yMouseLocal};
    }


    bool XHairArtist::draw(const PlottingWindow &graph2D) {
        if(!graph2D.isMouseIn()) return true;

        auto vpRect = graph2D.getViewport();
        auto region = graph2D.getRegion();

        fix mouseLocal = getMouseViewportCoord(vpRect);

        auto XHairLocation = FromViewportToSpaceCoord(mouseLocal, region, vpRect);

        auto label = graph2D.getXHairLabel(XHairLocation);

        auto currStyle = PlotThemeManager::GetCurrent();
        currStyle->ticksWriter->write(label, {(Real)mouseLocal.x+20, (Real)mouseLocal.y+20}, currStyle->graphNumbersColor);

        XHair.clear();
        XHair.addPoint({region.xMin, XHairLocation.y});
        XHair.addPoint({region.xMax, XHairLocation.y});
        XHair.addPoint({XHairLocation.x, region.yMin});
        XHair.addPoint({XHairLocation.x, region.yMax});

        return Graphics::OpenGL::Legacy::RenderPointSet(Slab::DummyPointer(XHair), currStyle->XHairStyle);
    }

} // Graphics