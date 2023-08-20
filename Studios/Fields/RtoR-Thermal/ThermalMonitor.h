//
// Created by joao on 7/12/22.
//

#ifndef STUDIOSLAB_THERMALMONITOR_H
#define STUDIOSLAB_THERMALMONITOR_H

#include "Models/KleinGordon/RtoR/RtoRMonitor.h"
#include "Mappings/R2toR/View/Artists/FlatFieldDisplay.h"
#include "Mappings/RtoR/View/Graphics/HistoryDisplay.h"


namespace RtoR::Thermal {

    class Monitor : public RtoR::Monitor {
        Real transientGuess = -1.0;

        Real u;

        Real barϕ;
        Real tau;
        Real tau_indirect;

    public:
        explicit Monitor(const NumericConfig &, KGEnergy &hamiltonian);

        void draw() override;

        void setTransientGuess(Real guess);

    protected:
        void handleOutput(const OutputPacket &outInfo) override;


    private:

        Spaces::PointSet temperature1HistoryData;
        Spaces::PointSet temperature2HistoryData;
        Spaces::PointSet temperature3HistoryData;

        Fields::Graphing::PointSetGraph mTemperaturesGraph;

        Spaces::PointSet histogramKData;
        Spaces::PointSet histogramGradData;
        Spaces::PointSet histogramVData;
        Spaces::PointSet histogramEData;

        Fields::Graphing::PointSetGraph mHistogramsGraphK;
        Fields::Graphing::PointSetGraph mHistogramsGraphGrad;
        Fields::Graphing::PointSetGraph mHistogramsGraphV;
        Fields::Graphing::PointSetGraph mHistogramsGraphE;

        RtoR::Graphics::HistoryDisplay mFullHistoryDisplay;
    };
}




#endif //STUDIOSLAB_THERMALMONITOR_H
