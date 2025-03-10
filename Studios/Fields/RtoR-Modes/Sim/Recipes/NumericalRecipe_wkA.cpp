//
// Created by joao on 1/09/23.
//

#include "NumericalRecipe_wkA.h"

#include <memory>

#include "../BoundaryCondition.h"

#include "Math/Constants.h"

#include "Core/SlabCore.h"
#include "../Monitor.h"
#include "Models/KleinGordon/KG-Solver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#define DONT_REGISTER false

namespace Modes {

    // using namespace Slab::Models;

    NumericalRecipe_wkA::NumericalRecipe_wkA(bool doRegister)
    : KGRtoR::KGRtoRBuilder("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        interface->addParameters({&BCSelection, &omega, &k, &A, &driving_force});

        if(doRegister) RegisterCLInterface(interface);
    }

    Math::Base::BoundaryConditions_ptr NumericalRecipe_wkA::getBoundary() {
        auto prototype = KGRtoR::KGRtoRBuilder::newFieldState();

        fix L = DynamicPointerCast<KGNumericConfig>(getNumericConfig())->getL();
        fix A_0 = this->A.getValue();
        fix dk = 2*M_PI/L;
        fix k_0 = dk*this->k.getValue();
        fix j = this->omega.getValue();
        fix ω = dk*j;

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

    void NumericalRecipe_wkA::notifyCLArgsSetupFinished() {
        CLInterfaceOwner::notifyCLArgsSetupFinished();

        auto config = DynamicPointerCast<KGNumericConfig>(getNumericConfig());

        fix L = config->getL();
        fix n = config->getn();
        fix a = config->getr();

        switch (*BCSelection) {
            case 0:
                this->setLaplacianFixedBC();
                break;
            case 1:
            case 2:
                this->setLaplacianPeriodicBC();
                break;
            default: NOT_IMPLEMENTED;
        }

        fix dk = 2*M_PI/L;
        fix k_0 = dk*this->k.getValue();
        if(*omega < 0) {
            fix m2 = *massSqr;
            fix ohm_temp = sqrt(k_0*k_0 + m2);
            omega.setValue(ohm_temp/dk);

            Log::Info() << "omega_n parameter is negative. Using KG dispersion relation for its value." << Log::Flush;
        }
        fix ω = dk*this->omega.getValue();
        fix period = 2*Constants::pi / ω;
        fix res = (period/L)*n;

        Log::Info() << "Setting wavenumber κL/2π=" << this->k.getValue() << " ==> κ=" << k << Log::Flush;
        Log::Info() << "Setting ang. freq  ωL/2π=" << this->omega.getValue() << " ==> ω=" << ω << Log::Flush;
        Log::Info() << "Relation Aκ²=" << A*k*k << Log::Flush;
        Log::Info() << "Setting wavenumber k=2π/λ = " << k << Log::Flush;
        Log::Info() << "Computed amplitude is A=ι/k² = " << A << Log::Flush;
        Log::Info() << "Setting ang. freq ω²=k²+m² = " << ω*ω << " ==> ω = " << ω << Log::Flush;
        Log::Info() << Log::BGWhite+Log::FGBlack << "  Technical sine resolution is " << res << " steps/cycle ("
                    << static_cast<int>(res * a) << " sites/linear period)g " << Log::ResetFormatting << Log::Flush;
    }

    void *NumericalRecipe_wkA::buildOpenGLOutput() {
        auto config = DynamicPointerCast<KGNumericConfig>(getNumericConfig());


        // fix amp = (*A) * 1.1;
        auto monitor = new Modes::Monitor(config, *static_cast<KGRtoR::KGEnergy *>(getHamiltonian()), "Modes monitor");

        fix k_0 = *k;
        fix A_0 = *A;
        fix m2 = *mass < 0 ? 4/(M_PI*A_0) : *mass**mass;
        fix ω = sqrt(k*k + m2);

        monitor->setInputModes({A_0}, {k_0}, {ω});

        return monitor;
    }

    Str NumericalRecipe_wkA::suggestFileName() const {
        const auto SEPARATOR = " ";

        StrVector params = {"omega", "k", "A"};
        // if(*BCSelection == 1) params.emplace_back("harmonic");

        auto strParams = interface->toString(params, SEPARATOR);
        return KGRtoR::KGRtoRBuilder::suggestFileName() + SEPARATOR + strParams;
    }

    Pointer<Base::FunctionT<Real, Real>> NumericalRecipe_wkA::getNonHomogenous() {
        if(*driving_force && squareWave == nullptr) squareWave = Slab::New<Modes::SquareWave>(1);

        return squareWave;
    }

} // Modes