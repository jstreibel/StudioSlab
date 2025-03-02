//
// Created by joao on 1/09/23.
//

#ifndef STUDIOSLAB_SIM_BUILDER_WKA_H
#define STUDIOSLAB_SIM_BUILDER_WKA_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBuilder.h"
#include "SquareWave.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;


    class NumericalRecipe_wkA final : public Slab::Models::KGRtoR::KGRtoRBuilder {
        IntegerParameter BCSelection = IntegerParameter(0,    "BC",              "Boundary conditions selection:"
                                                                                "\n\t0: sine signal with 'omega' angular frequency in vacuum IC."
                                                                                "\n\t1: IC sine wave with 'omega' angular frequency and 'wave_number' wave number."
                                                                                "\n\t2: Dead.");
        RealParameter A              = RealParameter   (1.0,  "A",               "Amplitude of input sine wave");
        // RealParameter omega          = RealParameter   (1.0,  "w,omega",         "Angular frequency of input sine wave");
        RealParameter omega          = RealParameter(1.0, "w,omega_n", "Angular frequency 'ω' harmonic number. Computed as ω=2πj/L.");
        // RealParameter k              = RealParameter   (1.0,  "k,wave_number",   "Wave number of input sine wave, if Wave BC is selected.");
        RealParameter k              = RealParameter(1.0, "k,harmonic", "Harmonic number 'n' of input sine wave. Wavenumber is computed as k=2πn/L ");

        RealParameter mass             = RealParameter   (-1.0,  "mass", "Mass of for computing angular frequency. If negative, mass is set to m²=4/(πA)");
        BoolParameter driving_force  = BoolParameter   (false,"F,driving_force", "Compute with non-homogenous driving force.");

        Pointer<Modes::SquareWave> squareWave;
    protected:
        auto buildOpenGLOutput() -> void * override;

    public:
        auto suggestFileName() const -> Str override;

    public:
        explicit NumericalRecipe_wkA(bool doRegister=true);

        auto getBoundary() -> Slab::Math::Base::BoundaryConditions_ptr override;

        Pointer<Base::FunctionT<Real, Real>> getNonHomogenous() override;

        auto notifyCLArgsSetupFinished() -> void override;
    };

} // Modes

#endif //STUDIOSLAB_SIM_BUILDER_WKA_H
