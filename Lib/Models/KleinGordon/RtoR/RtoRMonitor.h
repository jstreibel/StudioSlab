//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGL_H
#define V_SHAPE_RTOROUTPUTOPENGL_H

#include "Core/Graphics/Window/WindowContainer/WindowPanel.h"

#include "Math/Graph/OpenGLMonitor.h"

#include "Mappings/RtoR/Model/RtoRResizableDiscreteFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/Section1D.h"
#include "Mappings/RtoR/View/Graphics/HistoryDisplay.h"
#include "Mappings/RtoR/View/Graphics/GraphRtoR.h"

#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"
#include "Mappings/R2toR/Model/FunctionsCollection/CorrelationFunction/Sampler.h"
#include "Mappings/R2toR/Model/FunctionsCollection/CorrelationFunction/CorrelationFunction.h"

#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"

namespace RtoR {

    class Monitor : public Core::Graphics::OpenGLMonitor {
        bool showEnergyHistoryAsDensities;

        bool notifyKeyboard(unsigned char key, int x, int y) override;

        Real Δt = 0.0;
        RtoR2::StraightLine corrSampleLine;
        std::shared_ptr<R2toR::Sampler> sampler;
        RtoR::Section1D::Ptr mSpaceCorrelation;
        R2toR::CorrelationFunction mCorrelationFunction;
        GraphRtoR mCorrelationGraph;
        GraphRtoR mSpaceFourierModesGraph;

        RtoR::Section1D mHistorySectionFunc;

        GraphRtoR mHistoryGraph;

        std::shared_ptr<const R2toR::DiscreteFunction>
                  simulationHistory = nullptr;

        GraphRtoR mFieldsGraph;


        Spaces::PointSet UHistoryData;
        Spaces::PointSet KHistoryData;
        Spaces::PointSet WHistoryData;
        Spaces::PointSet VHistoryData;

        const Styles::PlotStyle U_style = Styles::GetColorScheme()->funcPlotStyles[0];
        const Styles::PlotStyle K_style = Styles::GetColorScheme()->funcPlotStyles[1];
        const Styles::PlotStyle W_style = Styles::GetColorScheme()->funcPlotStyles[2];
        const Styles::PlotStyle V_style = Styles::GetColorScheme()->funcPlotStyles[3];

        bool showPot = true;
        bool showKineticEnergy = false;
        bool showGradientEnergy = false;
        bool showEnergyDensity = false;

        float t_history = .0f;
        int step_history = 0;

        void updateHistoryGraphs();
        void updateFourierGraph();

    protected:
        RtoR::Graphics::HistoryDisplay mFullHistoryDisplay;

        Real U = .0;
        Real K = .0;
        Real V = .0;
        Real W = .0;
        KGEnergy &hamiltonian;
        Core::Graphics::Graph2D mEnergyGraph;

        void draw() override;

        void handleOutput(const OutputPacket &outInfo) override;

    public:
        Monitor(const NumericConfig &params,
                KGEnergy &hamiltonian,
                Real phiMin=-1,
                Real phiMax=1,
                Str name = "general graphic monitor",
                bool showEnergyHistoryAsDensities=false);

        virtual void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory);
    };

}


#endif //V_SHAPE_RTOROUTPUTOPENGL_H

