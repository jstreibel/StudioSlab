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

    using namespace Slab::Models;

    Builder::Builder(bool doRegister)
    : KGRtoR::KGRtoRBuilder("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        interface->addParameters({&BCSelection, &A, &omega, &k, &driving_force});

        if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
    }

    Math::Base::BoundaryConditions_ptr Builder::getBoundary() {
        auto prototype = KGRtoR::KGRtoRBuilder::newFieldState();

        fix L = this->getNumericParams().getL();

        fix A_0 = this->A.getValue();
        fix dk = M_PI/L;
        fix k_0 = dk*this->k.getValue();
        fix ω = dk*this->omega.getValue();

        if(*BCSelection == 0) return New <Modes::SignalBC> (prototype, A_0,  ω);
        if(*BCSelection == 1) {
            fix A2 = A_0 * 0.0;

            auto func1 = RtoR::Sine(A_0, k_0);
            auto func2 = RtoR::Sine(A2, 2*k_0);
            auto f_0 = New <RtoR::FunctionSummable> (func1, func2);

            auto ddtfunc1 = RtoR::Cosine(A*ω, k_0);
            auto ddtfunc2 = RtoR::Cosine(A2*(2*ω), 2*k_0);
            auto ddtf_0 = New <RtoR::FunctionSummable> (ddtfunc1, ddtfunc2);

            return New <KGRtoR::BoundaryCondition> (prototype, f_0, ddtf_0);
        }
        if(*BCSelection == 2){
            if(getNonHomogenous() == nullptr) getNonHomogenous();

            return New <Modes::DrivenBC> (prototype, squareWave);
        }

        throw Exception(Str("Unknown initial condition ") + ToStr(*BCSelection));
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

        fix A = this->A.getValue();
        fix dk = M_PI/L;
        fix k = dk*this->k.getValue();
        fix ω = dk*this->omega.getValue();
        Log::Info() << "Setting wavenumber κL/π=" << this->k.getValue() << " ==> κ=" << k << Log::Flush;
        Log::Info() << "Setting ang. freq  ωL/π=" << this->omega.getValue() << " ==> ω=" << ω << Log::Flush;
        Log::Info() << "Relation Aκ²=" << A*k*k << Log::Flush;
        Log::Info() << Log::BGWhite+Log::FGBlack << "  Technical sine resolution is " << res << " steps/cycle (" << int(res*a) << " sites/linear period)  " << Log::ResetFormatting << Log::Flush;
    }

    void *Builder::buildOpenGLOutput() {

        fix amp = (*A) * 1.1;
        auto monitor = new Modes::Monitor(simulationConfig.numericConfig, *(KGRtoR::KGEnergy*)getHamiltonian(), -amp, +amp, "Modes monitor");

        monitor->setInputModes({*A}, {*omega});

        return monitor;
    }

    Str Builder::suggestFileName() const {
        const auto SEPARATOR = " ";

        StrVector params = {"A", "omega_n"};
        if(*BCSelection == 1) params.emplace_back("harmonic");

        auto strParams = interface->toString(params, SEPARATOR);
        return KGRtoR::KGRtoRBuilder::suggestFileName() + SEPARATOR + strParams;
    }

    Pointer<Base::FunctionT<Real, Real>> Builder::getNonHomogenous() {
        if(*driving_force && squareWave == nullptr) squareWave = Slab::New<Modes::SquareWave>(1);

        return squareWave;
    }

} // Modes