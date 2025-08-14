//
// Created by joao on 21/05/24.
//

#include "RtoRFunctionArtist.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

#include <utility>
#include "Graphics/Plot2D/Util/PlotStyleGUI.h"

#include "3rdParty/ImGui.h"
#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"

namespace Slab::Graphics {
    RtoRFunctionArtist::RtoRFunctionArtist(RtoR::Function_ptr func, PlotStyle plotStyle, CountType samples)
    : function(std::move(func)), plotStyle(plotStyle), samples(samples)
    {    }

    bool RtoRFunctionArtist::Draw(const FPlot2DWindow &d) {
        if(function == nullptr) return true;

        d.RequireLabelOverlay(GetLabel(), Naked(plotStyle));

        glLineWidth(plotStyle.thickness);

        auto graphRect = d.GetRegion().getRect();

        {
            OpenGL::Legacy::FShaderGuard Guard{};
            OpenGL::Legacy::SetupOrtho(graphRect);
            OpenGL::Legacy::FunctionRenderere::RenderFunction(*function, plotStyle.lineColor, plotStyle.filled, graphRect.xMin,
                                                       graphRect.xMax, samples, 1);
        }

        return true;
    }

    bool RtoRFunctionArtist::HasGUI() {
        return true;
    }

    void RtoRFunctionArtist::DrawGUI() {
        int samps = (int)samples;
        if(ImGui::SliderInt(UniqueName("Samples").c_str(), &samps, 100, 100000))
            samples = (CountType)samps;

        DrawPlotStyleGUI(plotStyle, GetLabel());

        FArtist::DrawGUI();
    }

    void RtoRFunctionArtist::setFunction(RtoR::Function_ptr func) {
        function = func;
    }

    const RectR &RtoRFunctionArtist::GetRegion() {
        if(function != nullptr) {
            try {
                region.yMin = function->min();
                region.yMax = function->max();
                region.xMin = 0;
                region.xMax = 10;
            }
            catch (...) {

            }
        }

        return FArtist::GetRegion();
    }

    void RtoRFunctionArtist::setStyle(PlotStyle sty) { plotStyle = sty; }

    void RtoRFunctionArtist::setSampling(CountType samps) { samples = samps; }

} // Graphics