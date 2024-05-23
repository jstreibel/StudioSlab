//
// Created by joao on 21/05/24.
//

#include "ParametricCurve2DArtist.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

#include <utility>

namespace Slab::Graphics {

    using namespace Math;

    ParametricCurve2DArtist::ParametricCurve2DArtist(RtoR2::ParametricCurve_ptr curve, PlotStyle style)
    : curve(std::move(curve)), plotStyle(style) {

    }

    void ParametricCurve2DArtist::draw(const PlottingWindow &d) {
        auto pointSet = curve.get()->renderToPointSet();

        OpenGL::Legacy::RenderPointSet(pointSet, plotStyle);

    }

    void ParametricCurve2DArtist::setCurve(Slab::Pointer<RtoR2::ParametricCurve> curve) {
        this->curve = curve;
    }


} // Graphics