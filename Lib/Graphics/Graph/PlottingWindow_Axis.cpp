//
// Created by joao on 29/08/23.
//

#include "PlottingWindow.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

#include "PlotThemeManager.h"


namespace Slab {

    Str Graphics::PlottingWindow::getXHairLabel(const Point2D &coords) const {
        fix digits = 5;
        auto &hUnit = axisArtist.getHorizontalUnit();
        auto &vUnit = axisArtist.getVerticalUnit();

        Str hAxisName = axisArtist.getHorizontalAxisLabel();
        Str vAxisName = axisArtist.getVerticalAxisLabel();
        auto baseLabel = Str("(") + hAxisName + ", " + vAxisName + ") = (" + hUnit(coords.x, digits) + ", " +
                         vUnit(coords.y, digits) + ")";

        for (IN contie: content) {
            IN artie = contie.second;

            if (artie->isVisible() && artie->getRegion().doesHit(coords.x, coords.y)) {
                auto artLabel = artie->getXHairInfo(coords);

                if (artLabel == "") continue;

                baseLabel += " --> " + artLabel;
            }
        }

        return baseLabel;
    }

    void Graphics::PlottingWindow::reviewGraphRanges() {
        RectR newRegion = region.getRect();

        for (auto &a: content) {
            auto &artist = a.second;

            if(artist->affectsGraphRanges()) {
                newRegion = artist->getRegion();
                break;
            }
        }

        bool unaffected = true;
        for (auto &a: content) {
            auto &artist = a.second;

            if (!artist->affectsGraphRanges()) continue;

            auto aRegion = artist->getRegion();

            if (aRegion.xMax > newRegion.xMax){ newRegion.xMax = aRegion.xMax; unaffected = false; }
            if (aRegion.xMin < newRegion.xMin){ newRegion.xMin = aRegion.xMin; unaffected = false; }
            if (aRegion.yMax > newRegion.yMax){ newRegion.yMax = aRegion.yMax; unaffected = false; }
            if (aRegion.yMin < newRegion.yMin){ newRegion.yMin = aRegion.yMin; unaffected = false; }
        }

        if(Common::AreEqual(newRegion.xMin, newRegion.xMax)) newRegion.xMax = newRegion.xMin+1;
        if(Common::AreEqual(newRegion.yMin, newRegion.yMax)) newRegion.yMax = newRegion.yMin+1;

        if(!unaffected)
        {
            auto currStyle = PlotThemeManager::GetCurrent();
            auto pix = Slab::Graphics::PixelSizeInSpace(newRegion, getViewport());
            auto Δx = (Real) currStyle->hAxisPaddingInPixels * pix.x;
            auto Δy = (Real) currStyle->hAxisPaddingInPixels * pix.y;

            newRegion.xMin -= 2*Δx;
            newRegion.xMax += 2*Δx;
            newRegion.yMin -= 2*Δy;
            newRegion.yMax += 2*Δy;
        }

        region.animate_xMin(newRegion.xMin);
        region.animate_xMax(newRegion.xMax);
        region.animate_yMin(newRegion.yMin);
        region.animate_yMax(newRegion.yMax);
    }


}