//
// Created by joao on 10/11/24.
//

#ifndef STUDIOSLAB_TIMEFTVIEWER_H
#define STUDIOSLAB_TIMEFTVIEWER_H

#include "KGViewer.h"
#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"
#include "Models/KleinGordon/RtoR/Analytic/TwoPointCorrelation.h"

namespace Slab::Models::KGRtoR {

    class TimeFTViewer : public KGViewer {
        float t0=0, Δt=1;
        bool auto_update = false;

        Pointer<Graphics::RtoRFunctionArtist> timeDFTAverageArtist;
        CorrelationDecay powerDecayCorrelation;

        void compute();

    public:
        TimeFTViewer(const Pointer<Graphics::FGUIWindow> &guiWindow,
                     const Pointer<R2toR::FNumericFunction> &func=nullptr,
                     const Pointer<R2toR::FNumericFunction> &ddtFunc=nullptr);

        void Draw() override;

        void NotifyBecameVisible() override;


        void SetFunction(Pointer<Math::R2toR::FNumericFunction> function) override;

        void SetFunctionDerivative(FuncPointer pointer) override;

        Str GetName() const override;
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_TIMEFTVIEWER_H
