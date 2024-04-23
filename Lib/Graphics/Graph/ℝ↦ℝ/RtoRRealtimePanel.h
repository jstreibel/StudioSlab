//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORREALTIMEPANEL_H
#define STUDIOSLAB_RTORREALTIMEPANEL_H

#include "Graphics/OpenGL/OpenGLMonitor.h"
#include "GraphRtoR.h"

#include "Maps/RtoR2/StraightLine.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunction.h"

#include "Maps/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "HistoryDisplay.h"
#include "RtoRPanel.h"

namespace RtoR {
    class RealtimePanel : public Graphics::RtoRPanel {
    protected:
        bool showEnergyHistoryAsDensities;

    public:
        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

    protected:
        Graphics::GraphRtoR mSpaceFourierModesGraph;

        RtoR2::StraightLine historyLine;
        RtoR::Section1D mHistorySectionFunc;
        Graphics::GraphRtoR mHistorySliceGraph;

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
        void updateFourierGraph();

        ::Graphics::Graph2D mEnergyGraph;

        void draw() override;

        void handleOutput(const OutputPacket &outInfo) override;

    public:
        RealtimePanel(const NumericConfig &params,
                      KGEnergy &hamiltonian,
                      Graphics::GUIWindow &guiWindow,
                      const Real phiMin, const Real phiMax,
                      bool showEnergyHistoryAsDensities);

        void setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                  std::shared_ptr<Graphics::HistoryDisplay> simHistoryGraph) override;

        void setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                    const SimHistory_DFT::DFTDataHistory &dftData,
                                    std::shared_ptr<Graphics::HistoryDisplay> sftHistoryGraph) override;
    };
}

#endif //STUDIOSLAB_RTORREALTIMEPANEL_H
