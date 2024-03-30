//
// Created by joao on 1/10/23.
//

#include "XHairArtist.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/Graph/StylesManager.h"
#include "Graphics/Graph/Graph.h"
#include "imgui.h"

namespace Graphics {



    auto getMouseViewportCoord(const RectI &vpRect) -> Point2D {
        auto &guiBackend = Core::BackendManager::GetGUIBackend();

        fix &mouse = guiBackend.getMouseState();
        fix hScreen = guiBackend.getScreenHeight();

        fix xMouseLocal = mouse.x - vpRect.xMin;
        fix yMouseLocal = hScreen - mouse.y - vpRect.yMin;

        return {(Real) xMouseLocal, (Real) yMouseLocal};
    }


    void XHairArtist::draw(const Graph2D &graph2D) {
        if(!graph2D.isMouseIn()) return;

        auto vpRect = graph2D.getViewport();
        auto region = graph2D.getRegion();

        fix mouseLocal = getMouseViewportCoord(vpRect);

        auto XHairLocation = FromViewportToSpaceCoord(mouseLocal, region, vpRect);

        auto label = graph2D.getXHairLabel(XHairLocation);

        auto currStyle = Math::StylesManager::GetCurrent();
        currStyle->ticksWriter->write(label, {(Real)mouseLocal.x+20, (Real)mouseLocal.y+20}, currStyle->graphNumbersColor);

        XHair.clear();
        XHair.addPoint({region.xMin, XHairLocation.y});
        XHair.addPoint({region.xMax, XHairLocation.y});
        XHair.addPoint({XHairLocation.x, region.yMin});
        XHair.addPoint({XHairLocation.x, region.yMax});

        Graphics::Graph2D::renderPointSet(XHair, currStyle->XHairStyle);
    }

} // Graphics