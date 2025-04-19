//
// Created by joao on 25/03/25.
//

#include "SPI-Recipe.h"

#include <Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h>
#include <Math/Numerics/ODE/Odeint/OdeintRK4.h>
#include <Math/Numerics/ODE/Steppers/RungeKutta4.h>

#include "SPI-Solver.h"
#include "SPI-State.h"
#include "SPI-BC.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticOscillon1p1_FourierImpl.h"
#include "Math/Numerics/ODE/Steppers/Euler.h"

#include "Graphics/Window/SlabWindowManager.h"

#include "Models/KleinGordon/R2toR/R2toRBuilder.h"

namespace Slab::Models::StochasticPathIntegrals {

    const auto my_name = "Stochastic Path Integrals Recipe";
    const auto my_description = "Recipe for generating relevant field configurations in the (1+1) dimensional, real valued "
        "fields quantum path integral formalism.";

    SPIRecipe::SPIRecipe(const Pointer<SPINumericConfig> &numeric_config)
    : NumericalRecipe(numeric_config, my_name, my_description, true)
    , SPI_NumericConfig(numeric_config) {
    }

    auto SPIRecipe::buildOutputSockets() -> Base::OutputSockets {
        Base::OutputSockets sockets;

        auto monitor = New<KGR2toR::OutputOpenGL>(SPI_NumericConfig->getn());
        auto manager = Slab::New<Graphics::SlabWindowManager>();
        manager->addSlabWindow(monitor);

        Graphics::GetGraphicsBackend()->GetMainSystemWindow()->addAndOwnEventListener(manager);
        sockets.emplace_back(monitor);

        return sockets;
    }

    auto SPIRecipe::buildStepper() -> Pointer<Stepper> {
        fix L = SPI_NumericConfig->getL();
        fix t = SPI_NumericConfig->gett();
        fix N = SPI_NumericConfig->getN();
        fix dT = SPI_NumericConfig->getdT();

        fix xMin = - L/2;
        fix h = L / N;
        Fix tMin = 0.0;
        fix M = static_cast<Int>(floor(t / h));

        auto phi = New<R2toR::NumericFunction_CPU>(N, M, xMin, tMin, h, h);
        auto prototypeState = New<SPIState>(phi);

        auto du = New<SPIBC>(prototypeState);
        auto solver = New<SPISolver>(du);

        return New<Euler>(solver, dT);

        // return New<Odeint::StepperRK4>();

        // return New<RungeKutta4>(solver, dT);
    }
} // StochasticPathIntegrals::Models::Slab