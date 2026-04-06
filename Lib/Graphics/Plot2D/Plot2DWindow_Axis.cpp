//
// Created by joao on 29/08/23.
//

#include <ranges>
#include <algorithm>

#include "Plot2DWindow.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

#include "PlotThemeManager.h"


namespace Slab {

    Str Graphics::FPlot2DWindow::GetXHairLabel(const FPoint2D &coords) const {
        fix digits = 5;
        auto &hUnit = AxisArtist.getHorizontalUnit();
        auto &vUnit = AxisArtist.getVerticalUnit();

        Str hAxisName = AxisArtist.getHorizontalAxisLabel();
        Str vAxisName = AxisArtist.getVerticalAxisLabel();
        auto baseLabel = Str("(") + hAxisName + ", " + vAxisName + ") = (" + hUnit(coords.x, digits) + ", " +
                         vUnit(coords.y, digits) + ")";

        for (IN contie: Content) {
            IN artie = contie.second;

            if (artie->IsVisible() && artie->GetRegion().doesHit(coords.x, coords.y)) {
                auto artLabel = artie->GetXHairInfo(coords);

                if (artLabel.empty()) continue;

                baseLabel += " --> " + artLabel;
            }
        }

        return baseLabel;
    }

    void Graphics::FPlot2DWindow::ReviewGraphRanges() {
        RectR newRegion = Region.getRect();

        for (auto &a: Content) {
            auto &artist = a.second;

            if(artist->AffectsGraphRanges()) {
                newRegion = artist->GetRegion();
                break;
            }
        }

        bool unaffected = true;
        for (auto& Artist : Content | std::views::values) {
            if (!Artist->AffectsGraphRanges()) continue;

            auto aRegion = Artist->GetRegion();

            if (aRegion.xMax > newRegion.xMax){ newRegion.xMax = aRegion.xMax; unaffected = false; }
            if (aRegion.xMin < newRegion.xMin){ newRegion.xMin = aRegion.xMin; unaffected = false; }
            if (aRegion.yMax > newRegion.yMax){ newRegion.yMax = aRegion.yMax; unaffected = false; }
            if (aRegion.yMin < newRegion.yMin){ newRegion.yMin = aRegion.yMin; unaffected = false; }
        }

        if(Common::AreEqual(newRegion.xMin, newRegion.xMax)) newRegion.xMax = newRegion.xMin+1;
        if(Common::AreEqual(newRegion.yMin, newRegion.yMax)) newRegion.yMax = newRegion.yMin+1;

        if(!unaffected)
        {
            auto currStyle = FPlotThemeManager::GetCurrent();
            auto pix = Slab::Graphics::PixelSizeInSpace(newRegion, GetViewport());
            auto Δx = static_cast<DevFloat>(currStyle->hAxisPaddingInPixels) * pix.x;
            auto Δy = static_cast<DevFloat>(currStyle->hAxisPaddingInPixels) * pix.y;

            newRegion.xMin -= 2*Δx;
            newRegion.xMax += 2*Δx;
            newRegion.yMin -= 2*Δy;
            newRegion.yMax += 2*Δy;
        }

        if (LockUnitAspectRatio) {
            const auto viewport = GetViewport();
            const auto viewportWidth = static_cast<DevFloat>(std::max(1, viewport.GetWidth()));
            const auto viewportHeight = static_cast<DevFloat>(std::max(1, viewport.GetHeight()));
            const auto viewportAspect = viewportWidth / viewportHeight;
            if (viewportAspect > 0.0) {
                const auto yCenter = newRegion.yCenter();
                const auto halfWidth = static_cast<DevFloat>(0.5 * newRegion.GetWidth());
                const auto halfHeight = static_cast<DevFloat>(std::max(halfWidth / viewportAspect, DevFloat(1e-8)));
                newRegion.yMin = yCenter - halfHeight;
                newRegion.yMax = yCenter + halfHeight;
            }
        }

        Region.animate_xMin(newRegion.xMin);
        Region.animate_xMax(newRegion.xMax);
        Region.animate_yMin(newRegion.yMin);
        Region.animate_yMax(newRegion.yMax);
    }


}
