//
// Created by joao on 3/16/23.
//

#ifndef STUDIOSLAB_MONITOR_H
#define STUDIOSLAB_MONITOR_H


#include "Models/KleinGordon/RtoR/Graphics/RtoRMonitor.h"
#include "Math/Function/RtoR/Model/RtoRResizableNumericFunction.h"

namespace Montecarlo {
class Monitor : public Slab::Models::KGRtoR::Monitor {
        public:
        explicit Monitor(const Slab::Math::NumericConfig &params);
        void draw() override;

    private:
        Slab::Math::RtoR::ResizableNumericFunction temperature1History;
        Slab::Math::RtoR::ResizableNumericFunction temperature2History;
        Slab::Math::RtoR::ResizableNumericFunction temperature3History;

        Slab::Pointer<Slab::Graphics::PlottingWindow> mTemperaturesGraph;
        Slab::Pointer<Slab::Graphics::PlottingWindow> mHistogramsGraph;
        Slab::Pointer<Slab::Graphics::PlottingWindow> mHistogramsGraph2;
        Slab::Pointer<Slab::Graphics::PlottingWindow> mHistogramsGraph3;
        Slab::Pointer<Slab::Graphics::PlottingWindow> mHistogramsGraph4;

    };
}


#endif //STUDIOSLAB_MONITOR_H
