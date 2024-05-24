//
// Created by joao on 20/05/24.
//

#include "PointSetArtist.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Core/Tools/Log.h"

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
            auto pts = pointSet->getPoints();
            auto ptMax = pts[0];
            auto ptMin = pts[0];

            for(auto pt : pointSet->getPoints()) {
                if(ptMax.x<pt.x) ptMax.x = pt.x;
                if(ptMax.y<pt.y) ptMax.y = pt.y;

                if(ptMin.x<pt.x) ptMin.x = pt.x;
                if(ptMin.y<pt.y) ptMin.y = pt.y;
            }

            region = {ptMin.x, ptMax.x, ptMin.y, ptMax.y};
        }

        return Artist::getRegion();
    }


} // Graphics