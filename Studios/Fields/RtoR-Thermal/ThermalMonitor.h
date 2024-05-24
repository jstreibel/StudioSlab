//
// Created by joao on 7/12/22.
//

#ifndef STUDIOSLAB_THERMALMONITOR_H
#define STUDIOSLAB_THERMALMONITOR_H


#include "Graphics/Graph/ℝ↦ℝ/RtoRMonitor.h"
#include "Math/Function/RtoR2/StraightLine.h"

namespace Studios::Fields::RtoRThermal {

    using namespace Slab;

    class Monitor : public Math::RtoR::Monitor {
        Real transientGuess = -1.0;
        RtoR2::StraightLine transientLine;

        Real u=.0;

        Real barϕ=.0;
        Real tau=.0;
        Real tau_indirect=.0;

        Math::PointSet temperature1HistoryData;
        Math::PointSet temperature2HistoryData;
        Math::PointSet temperature3HistoryData;

        Graphics::PlottingWindow mTemperaturesGraph;

        Math::PointSet histogramKData;
        Math::PointSet histogramGradData;
        Math::PointSet histogramVData;
        Math::PointSet histogramEData;

        Graphics::PlottingWindow mHistogramsGraphK;
        Graphics::PlottingWindow mHistogramsGraphGrad;
        Graphics::PlottingWindow mHistogramsGraphV;
        Graphics::PlottingWindow mHistogramsGraphE;

    protected:
        void handleOutput(const OutputPacket &outInfo) override;

    public:
        explicit Monitor(const NumericConfig &, RtoR::KGEnergy &hamiltonian);

        void draw() override;

        void setTransientGuess(Real guess);
    };
}




#endif //STUDIOSLAB_THERMALMONITOR_H
