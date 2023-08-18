//
// Created by joao on 7/12/22.
//

#ifndef STUDIOSLAB_THERMALMONITOR_H
#define STUDIOSLAB_THERMALMONITOR_H

#include "Models/KleinGordon/RtoR/RtoRMonitor.h"

namespace RtoR {
    namespace Thermal {

        class Monitor : public RtoR::Monitor {
            Real transientGuess = -1.0;

        public:
            explicit Monitor(const NumericConfig &, KGEnergy &hamiltonian);

            void draw() override;

            void setTransientGuess(Real guess);

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

        };
    }

}


#endif //STUDIOSLAB_THERMALMONITOR_H
