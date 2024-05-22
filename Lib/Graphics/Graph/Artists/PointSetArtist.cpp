//
// Created by joao on 20/05/24.
//

#include "PointSetArtist.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

#include <utility>

namespace Graphics {

    PointSetArtist::PointSetArtist(Math::PointSet_ptr pointSet, PlotStyle style)
    : pointSet(std::move(pointSet)), plotStyle(style) {

    }

    void PointSetArtist::draw(const PlottingWindow &graph2D) {
        if(pointSet == nullptr) return;

        Graphics::OpenGL::Legacy::RenderPointSet(pointSet, plotStyle);
    }


} // Graphics