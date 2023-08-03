//
// Created by joao on 7/12/22.
//

#ifndef STUDIOSLAB_THERMALMONITOR_H
#define STUDIOSLAB_THERMALMONITOR_H

#include "Models/KleinGordon/RtoR/RtoRMonitor.h"

namespace RtoR {
    namespace Thermal {

        class Monitor : public RtoR::Monitor {
        public:
            explicit Monitor(const NumericParams &, KGEnergy &hamiltonian);

            void draw() override;

        private:
            Spaces::PointSet temperature1HistoryData;
            Spaces::PointSet temperature2HistoryData;
            Spaces::PointSet temperature3HistoryData;

            Fields::Graphing::PointSetGraph mTemperaturesGraph;
            Fields::Graphing::PointSetGraph mHistogramsGraph1;
            Fields::Graphing::PointSetGraph mHistogramsGraph2;
            Fields::Graphing::PointSetGraph mHistogramsGraph3;
            Fields::Graphing::PointSetGraph mHistogramsGraph4;

        };
    }

}


#endif //STUDIOSLAB_THERMALMONITOR_H
