//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORREALTIMEPANEL_H
#define STUDIOSLAB_RTORREALTIMEPANEL_H

#include "Graphics/OpenGL/OpenGLMonitor.h"
#include "../PlotThemeManager.h"

#include "Math/Function/RtoR2/StraightLine.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"

#include "Math/Function/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "RtoRPanel.h"

namespace Slab::Math::RtoR {

    class RealtimePanel : public Graphics::RtoRPanel {
    public:
        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

    protected:
        Graphics::PlottingWindow mFieldsGraph;
        Graphics::Artist_ptr vArtist;
        Graphics::Artist_ptr kArtist;
        Graphics::Artist_ptr wArtist;
        Graphics::Artist_ptr uArtist;

        Math::PointSet UHistoryData;
        Math::PointSet KHistoryData;
        Math::PointSet WHistoryData;
        Math::PointSet VHistoryData;

        const Graphics::PlotStyle U_style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0];
        const Graphics::PlotStyle K_style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
        const Graphics::PlotStyle W_style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[2];
        const Graphics::PlotStyle V_style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[3];

        bool showPot = true;
        bool showKineticEnergy = false;
        bool showGradientEnergy = false;
        bool showEnergyDensity = false;

        Graphics::PlottingWindow mEnergyGraph;

        void draw() override;

        void handleOutput(const OutputPacket &outInfo) override;

    public:
        RealtimePanel(const NumericConfig &params,
                      KGEnergy &hamiltonian,
                      Graphics::GUIWindow &guiWindow);

        void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                  Graphics::PlottingWindow_ptr simHistoryGraph) override;

        void setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                    const Models::DFTDataHistory &dftData,
                                    Graphics::PlottingWindow_ptr sftHistoryGraph) override;
    };
}

#endif //STUDIOSLAB_RTORREALTIMEPANEL_H
