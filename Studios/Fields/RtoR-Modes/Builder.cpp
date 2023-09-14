//
// Created by joao on 1/09/23.
//

#include "Builder.h"

#include "BoundaryCondition.h"

#include "Math/Constants.h"

#include "Core/Tools/Log.h"
#include "Core/Controller/Interface/InterfaceManager.h"
#include "Monitor.h"
#include "Models/KleinGordon/KGSolver.h"
#include "Maps/RtoR/Model/FunctionsCollection/Trigonometric.h"

#define DONT_REGISTER false

namespace Modes {

    Builder::Builder(bool doRegister)
    : RtoR::KGBuilder("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        interface->addParameters({&BCSelection, &A, &omega, &k});

        if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
    }

    void *Builder::getBoundary() {
        auto &prototype = *(RtoR::EquationState*) RtoR::KGBuilder::newFieldState();

        if(*BCSelection == 0) return new Modes::SignalBC(prototype, *A, *omega);
        if(*BCSelection == 1) return new RtoR::BoundaryCondition(prototype, new RtoR::Sine(*A, *k), new RtoR::Cosine(A**omega, *k));

        throw Str("Unknown initial condition ") + ToStr(*BCSelection);
    }

    void Builder::notifyCLArgsSetupFinished() {
        InterfaceOwner::notifyCLArgsSetupFinished();
        fix period = 2*Constants::pi / *omega;
        fix L = simulationConfig.numericConfig.getL();
        fix n = simulationConfig.numericConfig.getn();
        fix res = (period/L)*n;
        fix a = simulationConfig.numericConfig.getr();

        if(*BCSelection == 0) this->setLaplacianFixedBC();
        if(*BCSelection == 1) this->setLaplacianPeriodicBC();



        Log::Info() << Log::BGWhite+Log::FGBlack << "  Technical sine resolution is " << res << " steps/cycle (" << int(res*a) << " sites/linear period)  " << Log::ResetFormatting << Log::Flush;
    }

    RtoR::Monitor *Builder::buildOpenGLOutput() {
        fix amp = (*A) * 1.1;
        auto monitor = new Modes::Monitor(simulationConfig.numericConfig, *(RtoR::KGEnergy*)getHamiltonian(), -amp, +amp, "Modes monitor");

        monitor->setInputModes({*A}, {*omega});

        return monitor;
    }

} // Modes