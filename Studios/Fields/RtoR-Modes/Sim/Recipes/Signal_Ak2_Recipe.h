//
// Created by joao on 1/09/23.
//

#ifndef STUDIOSLAB_SIGNAL_Ak2_BUILDER_H
#define STUDIOSLAB_SIGNAL_Ak2_BUILDER_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoR-Recipe.h"
#include "../SquareWave.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;


    class Signal_Ak2_Recipe final : public Models::KGRtoR::FKGRtoR_Recipe {
        RealParameter A              = RealParameter   (1.0,  "A",               "Amplitude of input sine wave");
        RealParameter omega          = RealParameter(1.0, "w,omega", "Angular frequency 'ω=2πT' of input signal.");

    protected:
        auto BuildOpenGLOutput() -> void * override;

    public:
        auto SuggestFileName() const -> Str override;

        explicit Signal_Ak2_Recipe(bool doRegister=true);

        auto GetBoundary() -> Base::BoundaryConditions_ptr override;

        auto NotifyCLArgsSetupFinished() -> void override;
    };

} // Modes

#endif //STUDIOSLAB_SIGNAL_Ak2_BUILDER_H
