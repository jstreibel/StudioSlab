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
        currStyle->ticksWriter->write(label, {(Real)mouseLocal.x+20, h - (Real)mouseLocal.y+20}, currStyle->graphNumbersColor);

        XHair.clear();
        XHair.addPoint({region_rect.xMin, h-XHairLocation.y});
        XHair.addPoint({region_rect.xMax, h-XHairLocation.y});
        XHair.addPoint({XHairLocation.x, region_rect.yMin});
        XHair.addPoint({XHairLocation.x, region_rect.yMax});

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        OpenGL::Legacy::SetupOrtho(region_rect);

        return Graphics::OpenGL::Legacy::RenderPointSet(Slab::Naked(XHair), currStyle->XHairStyle);
    }

} // Graphics