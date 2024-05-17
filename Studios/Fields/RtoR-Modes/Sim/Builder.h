//
// Created by joao on 1/09/23.
//

#ifndef STUDIOSLAB_BUILDER_H
#define STUDIOSLAB_BUILDER_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"
#include "SquareWave.h"

namespace Modes {

    class Builder : public RtoR::KGBuilder {
        IntegerParameter BCSelection = IntegerParameter(0,    "BC",              "Boundary conditions selection:"
                                                                                "\n\t0: sine signal with 'omega' angular frequency in vacuum IC."
                                                                                "\n\t1: IC sine wave with 'omega' angular frequency and 'wave_number' wave number."
                                                                                "\n\t2: Dead.");
        RealParameter A              = RealParameter   (1.0,  "A",               "Amplitude of input sine wave");
        // RealParameter omega          = RealParameter   (1.0,  "w,omega",         "Angular frequency of input sine wave");
        RealParameter omega          = RealParameter(1.0, "w,omega_n", "Angular frequency 'ω' harmonic. Computed as ω=2πj/L");
        // RealParameter k              = RealParameter   (1.0,  "k,wave_number",   "Wave number of input sine wave, if Wave BC is selected.");
        RealParameter k              = RealParameter(1.0, "k,harmonic", "Harmonic number 'n' of input sine wave. Wavenumber is computed as k=2πn/L ");
        BoolParameter driving_force  = BoolParameter   (false,"F,driving_force", "Compute with non-homogenous driving force.");

        std::shared_ptr<Modes::SquareWave> squareWave;
    protected:
        auto buildOpenGLOutput() -> void * override;


    public:
        auto suggestFileName() const -> Str override;

    public:
        explicit Builder(bool doRegister=true);

        auto getBoundary() -> void * override;

        RtoR::Function::Ptr getNonHomogenous() override;

        auto notifyCLArgsSetupFinished() -> void override;
    };

} // Modes

#endif //STUDIOSLAB_BUILDER_H
