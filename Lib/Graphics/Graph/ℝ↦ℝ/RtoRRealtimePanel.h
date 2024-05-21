//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORREALTIMEPANEL_H
#define STUDIOSLAB_RTORREALTIMEPANEL_H

#include "Graphics/OpenGL/OpenGLMonitor.h"
#include "GraphRtoR.h"

#include "Math/Function/RtoR2/StraightLine.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/Section1D.h"

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
        RtoR2::StraightLine historyLine;
        RtoR::Section1D mHistorySectionFunc;

        Graphics::GraphRtoR mHistorySliceGraph;

        Graphics::Graph2D mFieldsGraph;
        Graphics::Artist_ptr vArtist;
        Graphics::Artist_ptr kArtist;
        Graphics::Artist_ptr wArtist;
        Graphics::Artist_ptr uArtist;

        Math::PointSet UHistoryData;
        Math::PointSet KHistoryData;
        Math::PointSet WHistoryData;
        Math::PointSet VHistoryData;

        const Graphics::PlotStyle U_style = Graphics::StylesManager::GetCurrent()->funcPlotStyles[0];
        const Graphics::PlotStyle K_style = Graphics::StylesManager::GetCurrent()->funcPlotStyles[1];
        const Graphics::PlotStyle W_style = Graphics::StylesManager::GetCurrent()->funcPlotStyles[2];
        const Graphics::PlotStyle V_style = Graphics::StylesManager::GetCurrent()->funcPlotStyles[3];

        bool showPot = true;
        bool showKineticEnergy = false;
        bool showGradientEnergy = false;
        bool showEnergyDensity = false;
        bool showComplexFourier = false;

        float t_history = .0f;
        int step_history = 0;

        void updateHistoryGraphs();

        ::Graphics::Graph2D mEnergyGraph;

        void draw() override;

        void handleOutput(const OutputPacket &outInfo) override;

    public:
        RealtimePanel(const NumericConfig &params,
                      KGEnergy &hamiltonian,
                      Graphics::GUIWindow &guiWindow,
                      const Real phiMin, const Real phiMax,
                      bool showEnergyHistoryAsDensities);

        void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                  Graphics::HistoryDisplay_ptr simHistoryGraph) override;

        void setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                    const DFTDataHistory &dftData,
                                    Graphics::HistoryDisplay_ptr sftHistoryGraph) override;
    };
}

#endif //STUDIOSLAB_RTORREALTIMEPANEL_H
