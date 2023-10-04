//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_RTOROUTPUTOPENGL_H
#define V_SHAPE_RTOROUTPUTOPENGL_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"

#include "Graphics/OpenGL/OpenGLMonitor.h"

#include "Maps/RtoR/Model/RtoRResizableDiscreteFunction.h"
#include "Maps/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"
#include "Maps/R2toR/Model/FunctionsCollection/CorrelationFunction/Sampler.h"
#include "Maps/R2toR/Model/FunctionsCollection/CorrelationFunction/CorrelationFunction.h"

#include "HistoryDisplay.h"
#include "Maps/RtoR2/StraightLine.h"
#include "Graphics/Graph/ℝ↦ℝ/GraphRtoR.h"
#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"

namespace RtoR {

    class Monitor : public ::Graphics::OpenGLMonitor {
    protected:
        bool showEnergyHistoryAsDensities;

    public:
        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

    protected:

        Real Δt = 0.0;
        RtoR2::StraightLine corrSampleLine;
        std::shared_ptr<R2toR::Sampler> sampler;
        RtoR::Section1D::Ptr mSpaceCorrelation;
        R2toR::CorrelationFunction mCorrelationFunction;
        Graphics::GraphRtoR mCorrelationGraph;
        Graphics::GraphRtoR mSpaceFourierModesGraph;

        RtoR::Section1D mHistorySectionFunc;

        Graphics::GraphRtoR mHistorySliceGraph;

        std::shared_ptr<const R2toR::DiscreteFunction>
                  simulationHistory = nullptr;
        std::shared_ptr<const R2toR::DiscreteFunction>
                spaceFTHistory = nullptr;

        Graphics::GraphRtoR mFieldsGraph;


        Spaces::PointSet UHistoryData;
        Spaces::PointSet KHistoryData;
        Spaces::PointSet WHistoryData;
        Spaces::PointSet VHistoryData;

        const Styles::PlotStyle U_style = Math::StylesManager::GetCurrent()->funcPlotStyles[0];
        const Styles::PlotStyle K_style = Math::StylesManager::GetCurrent()->funcPlotStyles[1];
        const Styles::PlotStyle W_style = Math::StylesManager::GetCurrent()->funcPlotStyles[2];
        const Styles::PlotStyle V_style = Math::StylesManager::GetCurrent()->funcPlotStyles[3];

        bool showPot = true;
        bool showKineticEnergy = false;
        bool showGradientEnergy = false;
        bool showEnergyDensity = false;
        bool showComplexFourier = false;

        float t_history = .0f;
        int step_history = 0;

        void updateHistoryGraphs();
        void updateFTHistoryGraph();
        void updateFourierGraph();

        Graphics::HistoryDisplay mFullHistoryDisplay;
        Graphics::HistoryDisplay mFullSpaceFTHistoryDisplay;

        Real U = .0;
        Real K = .0;
        Real V = .0;
        Real W = .0;
        KGEnergy &hamiltonian;
        ::Graphics::Graph2D mEnergyGraph;

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
        virtual void setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory);
    };

}


#endif //V_SHAPE_RTOROUTPUTOPENGL_H

