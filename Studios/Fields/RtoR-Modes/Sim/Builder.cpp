//
// Created by joao on 1/09/23.
//

#include "Builder.h"

#include <memory>

#include "BoundaryCondition.h"

#include "Math/Constants.h"

#include "Core/Tools/Log.h"
#include "Core/Controller/Interface/InterfaceManager.h"
#include "Monitor.h"
#include "Models/KleinGordon/KGSolver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#define DONT_REGISTER false

namespace Modes {

    Builder::Builder(bool doRegister)
    : RtoR::KGBuilder("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        interface->addParameters({&BCSelection, &A, &omega, &k, &driving_force});

        if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
    }

    void *Builder::getBoundary() {
        auto &prototype = *(RtoR::EquationState*) RtoR::KGBuilder::newFieldState();

        fix L = this->getNumericParams().getL();

        fix A = this->A.getValue();
        fix dk = 2*M_PI/L;
        fix k = dk*this->k.getValue();
        fix ω = dk*this->omega.getValue();

        if(*BCSelection == 0) return new Modes::SignalBC(prototype, A,  ω);
        if(*BCSelection == 1) {
            fix A2 = A * 0.0;

            auto func1 = RtoR::Sine(A, k);
            auto func2 = RtoR::Sine(A2, 2*k);
            auto f_0 = new RtoR::FunctionSummable(func1, func2);

            auto ddtfunc1 = RtoR::Cosine(A*ω, k);
            auto ddtfunc2 = RtoR::Cosine(A2*(2*ω), 2*k);
            auto ddtf_0 = new RtoR::FunctionSummable(ddtfunc1, ddtfunc2);

            return new RtoR::BoundaryCondition(prototype, f_0, ddtf_0);
        }
        if(*BCSelection == 2){
            if(getNonHomogenous() == nullptr) getNonHomogenous();

            return new Modes::DrivenBC(prototype, squareWave);
        }

        throw Str("Unknown initial condition ") + ToStr(*BCSelection);
    }

    void Builder::notifyCLArgsSetupFinished() {
        InterfaceOwner::notifyCLArgsSetupFinished();
        fix period = 2*Constants::pi / *omega;
        fix L = simulationConfig.numericConfig.getL();
        fix n = simulationConfig.numericConfig.getn();
        fix res = (period/L)*n;
        fix a = simulationConfig.numericConfig.getr();

        switch (*BCSelection) {
            case 0:
                this->setLaplacianFixedBC();
                break;
            case 1:
            case 2:
                this->setLaplacianPeriodicBC();
                break;
        }

        Log::Info() << Log::BGWhite+Log::FGBlack << "  Technical sine resolution is " << res << " steps/cycle (" << int(res*a) << " sites/linear period)  " << Log::ResetFormatting << Log::Flush;
    }

    void *Builder::buildOpenGLOutput() {
        fix amp = (*A) * 1.1;
        auto monitor = new Modes::Monitor(simulationConfig.numericConfig, *(RtoR::KGEnergy*)getHamiltonian(), -amp, +amp, "Modes monitor");

        monitor->setInputModes({*A}, {*omega});

        return monitor;
    }

    Str Builder::suggestFileName() const {
        const auto SEPARATOR = " ";

        StrVector params = {"A", "omega"};
        if(*BCSelection == 1) params.emplace_back("wave_number");

        auto strParams = interface->toString(params, SEPARATOR);
        return RtoR::KGBuilder::suggestFileName() + SEPARATOR + strParams;
    }

    Pointer<Base::FunctionT<Real, Real>> Builder::getNonHomogenous() {
        if(*driving_force == true && squareWave== nullptr) squareWave = Slab::New<Modes::SquareWave>(1);

        return squareWave;
    }

} // Modes