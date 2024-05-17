//
// Created by joao on 7/12/22.
//

#ifndef STUDIOSLAB_THERMALMONITOR_H
#define STUDIOSLAB_THERMALMONITOR_H


#include "Graphics/Graph/ℝ↦ℝ/RtoRMonitor.h"
#include "Math/Function/Maps/RtoR2/StraightLine.h"

namespace RtoR::Thermal {

    class Monitor : public ::RtoR::Monitor {
        Real transientGuess = -1.0;
        RtoR2::StraightLine transientLine;

        Real u=.0;

        Real barϕ=.0;
        Real tau=.0;
        Real tau_indirect=.0;

        Spaces::PointSet temperature1HistoryData;
        Spaces::PointSet temperature2HistoryData;
        Spaces::PointSet temperature3HistoryData;

        Graphics::Graph2D mTemperaturesGraph;

        Spaces::PointSet histogramKData;
        Spaces::PointSet histogramGradData;
        Spaces::PointSet histogramVData;
        Spaces::PointSet histogramEData;

        Graphics::Graph2D mHistogramsGraphK;
        Graphics::Graph2D mHistogramsGraphGrad;
        Graphics::Graph2D mHistogramsGraphV;
        Graphics::Graph2D mHistogramsGraphE;

    protected:
        void handleOutput(const OutputPacket &outInfo) override;

    public:
        explicit Monitor(const NumericConfig &, KGEnergy &hamiltonian);

        void draw() override;

        void setTransientGuess(Real guess);
    };
}




#endif //STUDIOSLAB_THERMALMONITOR_H
