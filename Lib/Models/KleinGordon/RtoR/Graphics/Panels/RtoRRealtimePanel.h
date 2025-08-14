//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORREALTIMEPANEL_H
#define STUDIOSLAB_RTORREALTIMEPANEL_H

#include "Math/Function/RtoR2/StraightLine.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Graphics/BaseMonitor.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREnergyCalculator.h"

#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"
#include "Graphics/ImGui/ImGui-SingleSlabWindow-Wrapper.h"
#include "Models/KleinGordon/KG-NumericConfig.h"

namespace Slab::Models::KGRtoR {

    class FRealtimePanel : public FRtoRPanel {

    public:
        FRealtimePanel(const TPointer<FKGNumericConfig> &Params,
                      FKGEnergy &Hamiltonian,
                      Graphics::FGUIWindow &GUIWindow);

        void SetSimulationHistory(TPointer<const R2toR::FNumericFunction>
 simulationHistory,
                                  const R2toRFunctionArtist_ptr &SimHistoryGraph) override;

        void SetSpaceFourierHistory(TPointer<const R2toR::FNumericFunction>
 SftHistory,
                                    const FDFTDataHistory &DFTData,
                                    const R2toRFunctionArtist_ptr &sftHistoryGraph) override;

    protected:
        FPlot2DWindow FieldsGraph;
        FArtist_ptr vArtist;
        FArtist_ptr kArtist;
        FArtist_ptr wArtist;
        FArtist_ptr uArtist;

        PointSet UHistoryData;
        PointSet KHistoryData;
        PointSet WHistoryData;
        PointSet VHistoryData;

        const PlotStyle U_style = PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        const PlotStyle K_style = PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        const PlotStyle W_style = PlotThemeManager::GetCurrent()->FuncPlotStyles[2];
        const PlotStyle V_style = PlotThemeManager::GetCurrent()->FuncPlotStyles[3];

        bool bShowPot = true;
        bool bShowKineticEnergy = false;
        bool bShowGradientEnergy = false;
        bool bShowEnergyDensity = false;

        FPlot2DWindow EnergyGraph;

        void ImmediateDraw(const FPlatformWindow&) override;

    private:
        void UpdateEnergyData();
        // Graphics::ImGuiWindow imGuiWindow;

    };
}

#endif //STUDIOSLAB_RTORREALTIMEPANEL_H
