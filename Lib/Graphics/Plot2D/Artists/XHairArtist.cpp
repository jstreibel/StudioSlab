//
// Created by joao on 1/10/23.
//

#include "XHairArtist.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"

namespace Slab::Graphics {

    bool XHairArtist::draw(const Plot2DWindow &graph2D) {
        if(!graph2D.isMouseIn()) return true;

        auto vpRect = graph2D.getViewport();
        const auto& region = graph2D.getRegion();
        auto region_rect = region.getRect();

        fix mouseLocal = graph2D.getMouseViewportCoord();
        fix h = vpRect.height();

        auto XHairLocation = FromViewportToSpaceCoord(mouseLocal, region_rect, vpRect);

        auto label = graph2D.getXHairLabel(XHairLocation);

        auto currStyle = PlotThemeManager::GetCurrent();
        currStyle->ticksWriter->write(label, {(DevFloat)mouseLocal.x+20, h - (DevFloat)mouseLocal.y+20}, currStyle->graphNumbersColor);

        XHair.clear();
        XHair.AddPoint({region_rect.xMin, XHairLocation.y});
        XHair.AddPoint({region_rect.xMax, XHairLocation.y});
        XHair.AddPoint({XHairLocation.x, region_rect.yMin});
        XHair.AddPoint({XHairLocation.x, region_rect.yMax});

        OpenGL::Legacy::SetupOrtho(region_rect);

        return Graphics::OpenGL::Legacy::RenderPointSet(Slab::Naked(XHair), currStyle->XHairStyle);
    }

} // Graphics