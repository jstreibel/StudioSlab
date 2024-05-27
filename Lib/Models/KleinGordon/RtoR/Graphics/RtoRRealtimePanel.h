//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORREALTIMEPANEL_H
#define STUDIOSLAB_RTORREALTIMEPANEL_H

#include "Math/Function/RtoR2/StraightLine.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Graphics/OpenGL/OpenGLMonitor.h"
#include "Graphics/Graph/PlotThemeManager.h"
#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"

#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"

#include "RtoRPanel.h"

namespace Slab::Models::KGRtoR {

    class RealtimePanel : public RtoRPanel {
        void updateEnergyData();

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

    public:
        RealtimePanel(const NumericConfig &params,
                      KGEnergy &hamiltonian,
                      Graphics::GUIWindow &guiWindow);

        void setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryGraph) override;

        void setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                    const DFTDataHistory &dftData,
                                    const R2toRFunctionArtist_ptr &sftHistoryGraph) override;


    };
}

#endif //STUDIOSLAB_RTORREALTIMEPANEL_H
