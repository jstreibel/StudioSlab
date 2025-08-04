//
// Created by joao on 1/09/23.
//

#ifndef STUDIOSLAB_MODES_Ak2_BUILDER_H
#define STUDIOSLAB_MODES_Ak2_BUILDER_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBuilder.h"
#include "../SquareWave.h"

namespace Modes {

    using namespace Slab;
    using namespace Slab::Math;


    class NumericalRecipe_Ak2 final : public Slab::Models::KGRtoR::KGRtoRBuilder {
        IntegerParameter BCSelection = IntegerParameter(0,    "BC",              "Boundary conditions selection:"
                                                                                "\n\t0: sine signal with 'omega' angular frequency in vacuum IC."
                                                                                "\n\t1: IC sine wave with 'omega' angular frequency and 'wave_number' wave number."
                                                                                "\n\t2: Dead.");
        // RealParameter A              = RealParameter   (1.0,  "A",               "Amplitude of input sine wave");
        // // RealParameter omega          = RealParameter   (1.0,  "w,omega",         "Angular frequency of input sine wave");
        // RealParameter omega          = RealParameter(1.0, "w,omega_n", "Angular frequency 'ω' harmonic. Computed as ω=2πj/L.");
        // // RealParameter k              = RealParameter   (1.0,  "k,wave_number",   "Wave number of input sine wave, if Wave BC is selected.");
        //RealParameter k              = RealParameter(1.0, "k,harmonic", "Harmonic number 'n' of input sine wave. Wavenumber is computed as k=2πn/L ");

        RealParameter Ak2              = RealParameter   (1.0,  "j,Ak2", "Parameter (iota), ι=Ak²");
        IntegerParameter wavelengths   = IntegerParameter(1,    "wavelengths", "Integer number of wavelengths within L");
        RealParameter mass             = RealParameter   (-1.0,  "mass", "Mass of for computing angular frequency. If negative, mass is set to 4k²/(πι)");
        BoolParameter driving_force  = BoolParameter   (false,"F,driving_force", "Compute with non-homogenous driving force.");

        Pointer<Modes::SquareWave> squareWave;
    protected:
        auto BuildOpenGLOutput() -> void * override;

    public:
        auto SuggestFileName() const -> Str override;

    public:
        explicit NumericalRecipe_Ak2(bool doRegister=true);

        auto GetBoundary() -> Slab::Math::Base::BoundaryConditions_ptr override;

        Pointer<Base::FunctionT<Real, Real>> GetNonHomogenousTerm() override;

        auto NotifyCLArgsSetupFinished() -> void override;
    };

} // Modes

#endif //STUDIOSLAB_MODES_Ak2_BUILDER_H
