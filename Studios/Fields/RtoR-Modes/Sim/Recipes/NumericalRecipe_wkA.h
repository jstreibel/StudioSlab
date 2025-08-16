//
// Created by joao on 1/09/23.
//

#ifndef STUDIOSLAB_MODES_WKA_BUILDER_H
#define STUDIOSLAB_MODES_WKA_BUILDER_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoR-Recipe.h"
#include "../SquareWave.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;


    class NumericalRecipe_wkA final : public Slab::Models::KGRtoR::FKGRtoR_Recipe {
        IntegerParameter BCSelection = IntegerParameter(0,  FParameterDescription{"BC", "Boundary conditions selection:"
                                                                                        "\n\t0: sine signal with 'omega' angular frequency in vacuum IC."
                                                                                        "\n\t1: IC sine wave with 'omega' angular frequency and 'wave_number' wave number."
                                                                                        "\n\t2: Dead."});
        RealParameter A              = RealParameter(1.0,   FParameterDescription{'A',                      "Amplitude of input sine wave"});
        RealParameter omega          = RealParameter(1.0,   FParameterDescription{'w',    "omega_n",        "Angular frequency 'ω' harmonic number. Computed as ω=2πj/L."});
        RealParameter k              = RealParameter(1.0,   FParameterDescription{'k',    "harmonic",       "Harmonic number 'n' of input sine wave. Wavenumber is computed as k=2πn/L "});
        RealParameter mass           = RealParameter(-1.0,  FParameterDescription{"mass",                   "Mass of for computing angular frequency. If negative, mass is set to m²=4/(πA)"});
        BoolParameter driving_force  = BoolParameter(false, FParameterDescription{'F',    "driving_force",  "Compute with non-homogenous driving force."});

        TPointer<Modes::SquareWave> squareWave;
    protected:
        auto BuildOpenGLOutput() -> void * override;

    public:
        [[nodiscard]] auto SuggestFileName() const -> Str override;

        explicit NumericalRecipe_wkA(bool doRegister=true);

        auto GetBoundary() -> Slab::Math::Base::BoundaryConditions_ptr override;

        TPointer<Base::FunctionT<DevFloat, DevFloat>> GetNonHomogenousTerm() override;

        auto NotifyCLArgsSetupFinished() -> void override;
    };

} // Modes

#endif //STUDIOSLAB_MODES_WKA_BUILDER_H
