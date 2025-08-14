//
// Created by joao on 1/10/23.
//

#include "XHairArtist.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"

namespace Slab::Graphics {

    bool XHairArtist::Draw(const FPlot2DWindow &PlotWindow) {
        if(!PlotWindow.IsMouseInside()) return true;

        auto vpRect = PlotWindow.GetViewport();
        const auto& region = PlotWindow.GetRegion();
        auto region_rect = region.getRect();

        fix MouseLocal = PlotWindow.GetMouseViewportCoord();
        fix h = vpRect.GetHeight();

        auto XHairLocation = FromViewportToSpaceCoord(MouseLocal, region_rect, vpRect);

        auto label = PlotWindow.GetXHairLabel(XHairLocation);

        auto currStyle = PlotThemeManager::GetCurrent();
        currStyle->TicksWriter->Write(label,
            {
                MouseLocal.x + 20,
                h - MouseLocal.y + 20
            }, currStyle->graphNumbersColor);

        XHair.clear();
        XHair.AddPoint({region_rect.xMin, XHairLocation.y});
        XHair.AddPoint({region_rect.xMax, XHairLocation.y});
        XHair.AddPoint({XHairLocation.x, region_rect.yMin});
        XHair.AddPoint({XHairLocation.x, region_rect.yMax});

        bool bResult;
        {
            OpenGL::Legacy::FShaderGuard Guard{};

            OpenGL::Legacy::SetupOrtho(region_rect);
            bResult = OpenGL::Legacy::RenderPointSett(Naked(XHair), currStyle->XHairStyle);
        }

        return bResult;
    }

} // Graphics