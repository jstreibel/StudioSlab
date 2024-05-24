//
// Created by joao on 20/05/24.
//

#include "PointSetArtist.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

#include "Graphics/Graph/PlottingWindow.h"

#include <utility>

namespace Slab::Graphics {

    PointSetArtist::PointSetArtist(Math::PointSet_ptr pointSet, PlotStyle style)
    : pointSet(std::move(pointSet)), plotStyle(style) {

    }

    bool PointSetArtist::draw(const PlottingWindow &graph2D) {
        if(pointSet == nullptr) return true;

        return Graphics::OpenGL::Legacy::RenderPointSet(pointSet, plotStyle);
    }

    const RectR &PointSetArtist::getRegion() {
        if(pointSet== nullptr || pointSet->getPoints().empty()) region = {-1,1,1,1};
        else {
            auto ptMax = pointSet->getMax();
            auto ptMin = pointSet->getMin();

            region = {ptMin.x, ptMax.x, ptMin.y, ptMax.y};
        }

        return Artist::getRegion();
    }


} // Graphics