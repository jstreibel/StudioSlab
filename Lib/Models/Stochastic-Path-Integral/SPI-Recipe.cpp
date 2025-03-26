//
// Created by joao on 25/03/25.
//

#include "SPI-Recipe.h"

#include <Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h>

#include "SPI-Solver.h"
#include "SPI-State.h"
#include "SPI-BC.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticOscillon1p1_FourierImpl.h"
#include "Math/Numerics/ODE/Steppers/Euler.h"
#include "Models/KleinGordon/R2toR/R2toRBuilder.h"

namespace Slab::Models::StochasticPathIntegrals {

    SPIRecipe::SPIRecipe(const Pointer<NumericConfig> &numeric_config, const Str &name,
        const Str &generalDescription, bool doRegister)
    : NumericalRecipe(numeric_config, name, generalDescription, false) {

        this->getInterface()->addParameters({&L, &t, &N, &dT, &nT});

        if (doRegister) registerToManager();
    }

    auto SPIRecipe::buildOutputSockets() -> Base::OutputSockets {
        Base::OutputSockets sockets;

        auto monitor = New<KGR2toR::OutputOpenGL>(*nT);

        return sockets;
    }

    auto SPIRecipe::buildStepper() -> Pointer<Stepper> {
        fix xMin = - *L/2;
        fix h = *L / *N;
        fix tMin = 0.0;
        fix M = static_cast<Int>(floor(*t / h));

        auto phi = New<R2toR::NumericFunction_CPU>(*N, M, xMin, tMin, h, h);
        auto prototypeState = New<SPIState>(phi);

        auto du = New<SPIBC>(prototypeState);
        auto solver = New<SPISolver>(du);

        return New<Euler>(solver, *dT);
    }
} // StochasticPathIntegrals::Models::Slab