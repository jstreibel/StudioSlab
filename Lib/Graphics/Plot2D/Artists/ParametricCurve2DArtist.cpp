//
// Created by joao on 21/05/24.
//

#include "ParametricCurve2DArtist.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

#include "Graphics/Plot2D/Plot2DWindow.h"

#include <utility>

#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"

namespace Slab::Graphics {

    using namespace Math;

    ParametricCurve2DArtist::ParametricCurve2DArtist(RtoR2::ParametricCurve_ptr curve, PlotStyle style)
    : curve(std::move(curve)), plotStyle(style) {

    }

    bool ParametricCurve2DArtist::Draw(const FPlot2DWindow &PlotWindow) {
        if(curve == nullptr) return false;

        OpenGL::Legacy::FShaderGuard Guard{};

        OpenGL::Legacy::SetupOrtho(PlotWindow.GetRegion().getRect());

        auto PointSet = curve.get()->renderToPointSet();

        bool bReturnValue = OpenGL::Legacy::RenderPointSet(PointSet, plotStyle);

        return bReturnValue;
    }

    void ParametricCurve2DArtist::setCurve(Slab::TPointer<RtoR2::ParametricCurve> curve) {
        this->curve = std::move(curve);
    }


} // Graphics