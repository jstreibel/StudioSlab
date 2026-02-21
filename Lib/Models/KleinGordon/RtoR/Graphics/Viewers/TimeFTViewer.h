//
// Created by joao on 10/11/24.
//

#ifndef STUDIOSLAB_TIMEFTVIEWER_H
#define STUDIOSLAB_TIMEFTVIEWER_H

#include "KGViewer.h"
#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"
#include "Models/KleinGordon/RtoR/Analytic/TwoPointCorrelation.h"

namespace Slab::Models::KGRtoR {

    class FTimeFTViewer : public FKGViewer {
        float t0=0, Δt=1;
        bool auto_update = false;

        TPointer<Graphics::RtoRFunctionArtist> timeDFTAverageArtist;
        CorrelationDecay powerDecayCorrelation;

        void compute();

    public:
        FTimeFTViewer(const TPointer<Graphics::FGUIWindow> &guiWindow,
                     const TPointer<R2toR::FNumericFunction> &func=nullptr,
                     const TPointer<R2toR::FNumericFunction> &ddtFunc=nullptr);

        void ImmediateDraw(const Graphics::FPlatformWindow&) override;

        void NotifyBecameVisible() override;


        void SetFunction(TPointer<Math::R2toR::FNumericFunction> function) override;

        void SetFunctionDerivative(FuncPointer pointer) override;

        Str GetName() const override;
    };

    using TimeFTViewer [[deprecated("Use FTimeFTViewer")]] = FTimeFTViewer;

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_TIMEFTVIEWER_H
