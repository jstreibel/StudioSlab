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
    : KGRtoR::FKGRtoR_Recipe("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        Interface->AddParameters({&BCSelection, &omega, &k, &A, &driving_force});

        if(doRegister) RegisterCLInterface(Interface);
    }

    Math::Base::BoundaryConditions_ptr NumericalRecipe_wkA::GetBoundary() {
        auto prototype = KGRtoR::FKGRtoR_Recipe::NewFieldState();

        fix L = DynamicPointerCast<KGNumericConfig>(getNumericConfig())->GetL();
        fix A_0 = this->A.GetValue();
        fix dk = 2*M_PI/L;
        fix k_0 = dk*this->k.GetValue();
        fix j = this->omega.GetValue();
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
            if(GetNonHomogenousTerm() == nullptr) GetNonHomogenousTerm();

            return New <Modes::DrivenBC> (prototype, squareWave);
        }

        throw Exception(Str("Unknown initial condition ") + ToStr(*BCSelection));
    }

    void NumericalRecipe_wkA::NotifyCLArgsSetupFinished() {
        FCommandLineInterfaceOwner::NotifyCLArgsSetupFinished();

        auto config = DynamicPointerCast<KGNumericConfig>(getNumericConfig());

        fix L = config->GetL();
        fix n = config->getn();
        fix a = config->getr();

        switch (*BCSelection) {
            case 0:
                this->SetLaplacianFixedBC();
                break;
            case 1:
            case 2:
                this->SetLaplacianPeriodicBC();
                break;
            default: NOT_IMPLEMENTED;
        }

        fix dk = 2*M_PI/L;
        fix k_0 = dk*this->k.GetValue();
        if(*omega < 0) {
            fix m2 = *massSqr;
            fix ohm_temp = sqrt(k_0*k_0 + m2);
            omega.SetValue(ohm_temp/dk);

            Log::Info() << "omega_n parameter is negative. Using KG dispersion relation for its value." << Log::Flush;
        }
        fix ω = dk*this->omega.GetValue();
        fix period = 2*Constants::pi / ω;
        fix res = (period/L)*n;

        Log::Info() << "Setting wavenumber κL/2π=" << this->k.GetValue() << " ==> κ=" << k << Log::Flush;
        Log::Info() << "Setting ang. freq  ωL/2π=" << this->omega.GetValue() << " ==> ω=" << ω << Log::Flush;
        Log::Info() << "Relation Aκ²=" << A*k*k << Log::Flush;
        Log::Info() << "Setting wavenumber k=2π/λ = " << k << Log::Flush;
        Log::Info() << "Computed amplitude is A=ι/k² = " << A << Log::Flush;
        Log::Info() << "Setting ang. freq ω²=k²+m² = " << ω*ω << " ==> ω = " << ω << Log::Flush;
        Log::Info() << Log::BGWhite+Log::FGBlack << "  Technical sine resolution is " << res << " steps/cycle ("
                    << static_cast<int>(res * a) << " sites/linear period)g " << Log::ResetFormatting << Log::Flush;
    }

    void *NumericalRecipe_wkA::BuildOpenGLOutput() {
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

    Str NumericalRecipe_wkA::SuggestFileName() const {
        const auto SEPARATOR = " ";

        const StrVector Params = {"omega_n", "k", "A"};
        // if(*BCSelection == 1) params.emplace_back("harmonic");

        fix StringRenderedParams = Interface->ToString(Params, SEPARATOR);
        return KGRtoR::FKGRtoR_Recipe::SuggestFileName() + SEPARATOR + StringRenderedParams;
    }

    TPointer<Base::FunctionT<DevFloat, DevFloat>> NumericalRecipe_wkA::GetNonHomogenousTerm() {
        if(*driving_force && squareWave == nullptr) squareWave = Slab::New<Modes::SquareWave>(1);

        return squareWave;
    }

} // Modes