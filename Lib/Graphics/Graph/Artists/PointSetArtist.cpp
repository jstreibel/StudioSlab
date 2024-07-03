//
// Created by joao on 20/05/24.
//

#include "PointSetArtist.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

#include "Graphics/Graph/PlottingWindow.h"

#include <utility>
#include "PlotStyleGUI.h"

namespace Slab::Graphics {

    PointSetArtist::PointSetArtist(Math::PointSet_ptr pointSet, PlotStyle style)
    : pointSet(std::move(pointSet)), plotStyle(style) {

    }

    auto PointSetArtist::draw(const PlottingWindow &graph2D) -> bool {
        if(pointSet == nullptr) return true;

        return Graphics::OpenGL::Legacy::RenderPointSet(pointSet, plotStyle);
    }

    auto PointSetArtist::getRegion() -> const RectR & {
        if(pointSet== nullptr || pointSet->getPoints().empty()) region = {-1,1,-1,1};
        else {
            auto ptMax = pointSet->getMax();
            auto ptMin = pointSet->getMin();

            region = {ptMin.x, ptMax.x, ptMin.y, ptMax.y};
        }

        return Artist::getRegion();
    }

    auto PointSetArtist::setPointSet(Math::PointSet_ptr newPointSet) -> void {
        pointSet = std::move(newPointSet);
    }

    auto PointSetArtist::setStyle(PlotStyle style) -> void {
        plotStyle = style;
    }

    void PointSetArtist::drawGUI() {
        DrawPlotStyleGUI(plotStyle, getLabel());
    }

    bool PointSetArtist::hasGUI() { return true; }


} // Graphics