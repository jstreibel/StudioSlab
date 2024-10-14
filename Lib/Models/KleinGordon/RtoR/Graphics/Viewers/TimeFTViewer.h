//
// Created by joao on 10/11/24.
//

#ifndef STUDIOSLAB_TIMEFTVIEWER_H
#define STUDIOSLAB_TIMEFTVIEWER_H

#include "KGViewer.h"
#include "Graphics/Graph/Artists/RtoRFunctionArtist.h"
#include "Models/KleinGordon/RtoR/Analytic/TwoPointCorrelation.h"

namespace Slab::Models::KGRtoR {

    class TimeFTViewer : public KGViewer {
        float t0=0, Δt=1;
        bool auto_update = false;

        Pointer<Graphics::RtoRFunctionArtist> timeDFTAverageArtist;
        CorrelationDecay powerDecayCorrelation;

        void compute();

    public:
        TimeFTViewer(const Pointer<Graphics::GUIWindow> &guiWindow,
                     const Pointer<R2toR::NumericFunction> &func=nullptr,
                     const Pointer<R2toR::NumericFunction> &ddtFunc=nullptr);

        void draw() override;

        void notifyBecameVisible() override;


        void setFunction(Pointer<Math::R2toR::NumericFunction> function) override;

        void setFunctionDerivative(FuncPointer pointer) override;

        Str getName() const override;
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_TIMEFTVIEWER_H
