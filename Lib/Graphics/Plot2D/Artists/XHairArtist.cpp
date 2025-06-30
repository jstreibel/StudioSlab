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

    bool XHairArtist::Draw(const FPlot2DWindow &graph2D) {
        if(!graph2D.IsMouseIn()) return true;

        auto vpRect = graph2D.GetViewport();
        const auto& region = graph2D.GetRegion();
        auto region_rect = region.getRect();

        fix mouseLocal = graph2D.GetMouseViewportCoord();
        fix h = vpRect.GetHeight();

        auto XHairLocation = FromViewportToSpaceCoord(mouseLocal, region_rect, vpRect);

        auto label = graph2D.GetXHairLabel(XHairLocation);

        auto currStyle = PlotThemeManager::GetCurrent();
        currStyle->TicksWriter->Write(label, {(DevFloat)mouseLocal.x+20, h - (DevFloat)mouseLocal.y+20}, currStyle->graphNumbersColor);

        XHair.clear();
        XHair.AddPoint({region_rect.xMin, XHairLocation.y});
        XHair.AddPoint({region_rect.xMax, XHairLocation.y});
        XHair.AddPoint({XHairLocation.x, region_rect.yMin});
        XHair.AddPoint({XHairLocation.x, region_rect.yMax});

        OpenGL::Legacy::PushLegacyMode();
        OpenGL::Legacy::SetupOrtho(region_rect);
        bool bResult = OpenGL::Legacy::RenderPointSett(Naked(XHair), currStyle->XHairStyle);
        OpenGL::Legacy::RestoreFromLegacyMode();

        return bResult;
    }

} // Graphics