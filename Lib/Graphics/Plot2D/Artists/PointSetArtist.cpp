//
// Created by joao on 20/05/24.
//

#include "PointSetArtist.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"

#include "Graphics/Plot2D/Plot2DWindow.h"

#include <utility>

#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/Plot2D/Util/PlotStyleGUI.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"

namespace Slab::Graphics {

    PointSetArtist::PointSetArtist(Math::PointSet_ptr pointSet, PlotStyle style)
    : pointSet(std::move(pointSet)), plotStyle(style) {

    }

    auto PointSetArtist::Draw(const FPlot2DWindow &graph2D) -> bool {
        if(pointSet == nullptr) return true;

        graph2D.RequireLabelOverlay(GetLabel(), Naked(plotStyle));

        OpenGL::Legacy::FShaderGuard Guard{};

        OpenGL::Legacy::SetupOrtho(graph2D.GetRegion().getRect());

        bool bResult = OpenGL::Legacy::RenderPointSet(pointSet, plotStyle);

        return bResult;
    }

    auto PointSetArtist::GetRegion() -> const RectR & {
        if(pointSet== nullptr || pointSet->getPoints().empty()) region = {-1,1,-1,1};
        else {
            auto ptMax = pointSet->getMax();
            auto ptMin = pointSet->getMin();

            region = {ptMin.x, ptMax.x, ptMin.y, ptMax.y};
        }

        return FArtist::GetRegion();
    }

    auto PointSetArtist::setPointSet(Math::PointSet_ptr newPointSet) -> void {
        pointSet = std::move(newPointSet);
    }

    auto PointSetArtist::setStyle(PlotStyle style) -> void {
        plotStyle = style;
    }

    void PointSetArtist::DrawGUI() {
        DrawPlotStyleGUI(plotStyle, GetLabel());
    }

    bool PointSetArtist::HasGUI() { return true; }


} // Graphics