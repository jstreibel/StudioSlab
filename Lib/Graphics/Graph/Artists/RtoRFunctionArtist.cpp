//
// Created by joao on 21/05/24.
//

#include "RtoRFunctionArtist.h"
#include "Graphics/OpenGL/LegacyGL/FunctionRenderer.h"
#include "Graphics/Graph/PlottingWindow.h"

#include <utility>
#include "Graphics/Graph/Util/PlotStyleGUI.h"

#include "3rdParty/ImGui.h"

namespace Slab::Graphics {
    RtoRFunctionArtist::RtoRFunctionArtist(RtoR::Function_ptr func, PlotStyle plotStyle, Count samples)
    : function(std::move(func)), plotStyle(plotStyle), samples(samples)
    {    }

    bool RtoRFunctionArtist::draw(const PlottingWindow &d) {
        if(function == nullptr) return true;

        d.requireLabelOverlay(getLabel(), Naked(plotStyle));

        glLineWidth(plotStyle.thickness);

        auto graphRect = d.getRegion().getRect();
        Graphics::FunctionRenderer::renderFunction(*function, plotStyle.lineColor, plotStyle.filled, graphRect.xMin,
                                                   graphRect.xMax, samples, 1);

        return true;
    }

    bool RtoRFunctionArtist::hasGUI() {
        return true;
    }

    void RtoRFunctionArtist::drawGUI() {
        int samps = (int)samples;
        if(ImGui::SliderInt(UniqueName("Samples").c_str(), &samps, 100, 100000))
            samples = (Count)samps;

        DrawPlotStyleGUI(plotStyle, getLabel());

        Artist::drawGUI();
    }

    void RtoRFunctionArtist::setFunction(RtoR::Function_ptr func) {
        function = func;
    }

    const RectR &RtoRFunctionArtist::getRegion() {
        try {
            region.yMin = function->min();
            region.yMax = function->max();
            region.xMin = 0;
            region.xMax = 10;
        }
        catch (...) {
            
        }
        

        return Artist::getRegion();
    }
} // Graphics