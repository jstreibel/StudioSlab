//
// Created by joao on 16/04/24.
//

#ifndef STUDIOSLAB_RTOR_STATISTICAL_MONITOR_H
#define STUDIOSLAB_RTOR_STATISTICAL_MONITOR_H


#include "Graphics/BaseMonitor.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

#include "Math/Function/Function.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Math/Function/RtoR2/StraightLine.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREnergyCalculator.h"
#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/CorrelationFunction/Sampler.h"
#include "Utils/Threads.h"


namespace Slab::Models::KGRtoR {

    class RtoRStatisticsPanel final : public FRtoRPanel {

        TPointer<RtoR2::StraightLine>
 correlationLine;
        RtoR::Section1D_ptr mSpaceCorrelation;
        R2toR::FNumericFunction_ptr mCorrelationComputed;
        Graphics::FPlot2DWindow mCorrelationGraph;

        DevFloat transientHint = -1.0;

        KGEnergy &hamiltonian;

        DevFloat u=.0;

        DevFloat barϕ=.0;
        DevFloat tau=.0;
        DevFloat tau_indirect=.0;
        DevFloat tau_avg=.0;

        Math::PointSet temperature1HistoryData;
        Math::PointSet temperature2HistoryData;
        Math::PointSet temperature3HistoryData;
        Math::PointSet temperature4HistoryData;

        Math::PointSet histogramKData;
        Math::PointSet histogramGradData;
        Math::PointSet histogramVData;
        Math::PointSet histogramEData;

        void drawGUI();

        void updateEnergyData();

    public:
        RtoRStatisticsPanel(const TPointer<KGNumericConfig> &params, KGEnergy &hamiltonian, Graphics::FGUIWindow &guiWindow);

        void ImmediateDraw(const FPlatformWindow&) override;

        void SetTransientHint(DevFloat);

        void SetSimulationHistory(TPointer<const R2toR::FNumericFunction>
 simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryArtist) override;
    };
}


#endif //STUDIOSLAB_RTOR_STATISTICAL_MONITOR_H