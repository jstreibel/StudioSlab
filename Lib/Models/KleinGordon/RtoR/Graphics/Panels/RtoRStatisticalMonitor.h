//
// Created by joao on 16/04/24.
//

#ifndef STUDIOSLAB_RTORSTATISTICALMONITOR_H
#define STUDIOSLAB_RTORSTATISTICALMONITOR_H


#include "Graphics/BaseMonitor.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

#include "Math/Function/Function.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Section1D.h"

#include "Math/Function/RtoR2/StraightLine.h"

#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "Models/KleinGordon/RtoR/Graphics/RtoRPanel.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/CorrelationFunction/Sampler.h"
#include "Utils/Threads.h"


namespace Slab::Models::KGRtoR {

    class RtoRStatisticsPanel : public RtoRPanel {

        RtoR2::StraightLine_ptr correlationLine;
        RtoR::Section1D_ptr mSpaceCorrelation;
        R2toR::NumericFunction_ptr mCorrelationComputed;
        Graphics::Plot2DWindow mCorrelationGraph;

        Real transientHint = -1.0;

        KGEnergy &hamiltonian;

        Real u=.0;

        Real barϕ=.0;
        Real tau=.0;
        Real tau_indirect=.0;
        Real tau_avg=.0;

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
        RtoRStatisticsPanel(const NumericConfig &params, KGEnergy &hamiltonian, Graphics::GUIWindow &guiWindow);

        void draw() override;

        void setTransientHint(Real);

        void setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                  const R2toRFunctionArtist_ptr &simHistoryArtist) override;
    };
}


#endif //STUDIOSLAB_RTORSTATISTICALMONITOR_H
