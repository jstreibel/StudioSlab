//
// Created by joao on 1/09/23.
//

#include "NumericalRecipe_Ak2.h"

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

    NumericalRecipe_Ak2::NumericalRecipe_Ak2(bool doRegister)
    : KGRtoR::FKGRtoR_Recipe("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        Interface->AddParameters({&BCSelection, &Ak2, &wavelengths, &driving_force});

        if(doRegister) RegisterCLInterface(Interface);
    }

    Math::Base::BoundaryConditions_ptr NumericalRecipe_Ak2::GetBoundary() {
        auto prototype = KGRtoR::FKGRtoR_Recipe::NewFieldState();

        fix L = DynamicPointerCast<FKGNumericConfig>(GetNumericConfig())->GetL();
        fix λ = L / (DevFloat)*wavelengths;;
        fix ι = *Ak2;
        fix k = 2*M_PI/λ;
        fix A = ι/(k*k);
        fix m2 = *mass < 0 ? 4*k*k / (M_PI*ι) : *mass**mass;
        fix ω = sqrt(k*k + m2);

        if(*BCSelection == 0) return New <Modes::SignalBC> (prototype, A,  ω);
        if(*BCSelection == 1) {
            fix A2 = A * 0.0;

            auto func1 = RtoR::Sine(A, k);
            auto func2 = RtoR::Sine(A2, 2*k);
            auto f_0 = New <RtoR::FunctionSummable> (func1, func2);

            auto ddtfunc1 = RtoR::Cosine(A*ω, k);
            auto ddtfunc2 = RtoR::Cosine(A2*(2*ω), 2*k);
            auto ddtf_0 = New <RtoR::FunctionSummable> (ddtfunc1, ddtfunc2);

            return New <KGRtoR::BoundaryCondition> (prototype, f_0, ddtf_0);
        }
        if(*BCSelection == 2){
            if(GetNonHomogenousTerm() == nullptr) GetNonHomogenousTerm();

            return New <Modes::DrivenBC> (prototype, squareWave);
        }

        throw Exception(Str("Unknown initial condition ") + ToStr(*BCSelection));
    }

    void NumericalRecipe_Ak2::NotifyInterfaceSetupIsFinished() {
        FInterfaceOwner::NotifyInterfaceSetupIsFinished();

        auto config = DynamicPointerCast<FKGNumericConfig>(GetNumericConfig());

        fix L = config->GetL();
        fix n = config->getn();
        fix a = config->Get_r();

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

        fix λ = L / *wavelengths;
        fix ι = *Ak2;
        fix k = 2*M_PI/λ;
        fix A = ι/(k*k);
        fix m2 = *mass < 0 ? 4*k*k / (M_PI*ι) : *mass**mass;
        fix ω = sqrt(k*k + m2);

        fix period = 2*Constants::pi / ω;
        fix res = (period/L)*n;

        Log::Info() << "Relation Ak² = " << *Ak2 << " [input] " << Log::Flush;
        Log::Info() << "Wavelengths in L: " << *wavelengths << " [input] " << Log::Flush;
        Log::Info() << "Setting wavelength λ=L/wavelengths = " << λ << Log::Flush;
        Log::Info() << "Setting wavenumber k=2π/λ = " << k << Log::Flush;
        Log::Info() << "Computed amplitude is A=ι/k² = " << A << Log::Flush;
        Log::Info() << "Computed mass is m²=4/(πA) = " << m2 << " ==> m = " << sqrt(m2) << Log::Flush;
        Log::Info() << "Setting ang. freq ω²=k²+m² = " << ω*ω << " ==> ω = " << ω << Log::Flush;
        Log::Info() << Log::BGWhite+Log::FGBlack << "  Technical sine resolution is " << res << " steps/cycle (" << int(res*a) << " sites/linear period)  " << Log::ResetFormatting << Log::Flush;
    }

    void *NumericalRecipe_Ak2::BuildOpenGLOutput() {
        auto config = DynamicPointerCast<FKGNumericConfig>(GetNumericConfig());


        // fix amp = (*A) * 1.1;
        auto monitor = new Modes::Monitor(config, *(KGRtoR::FKGEnergy*)getHamiltonian(), "Modes monitor");

        fix L = config->GetL();
        fix λ = L / *wavelengths;
        fix ι = *Ak2;
        fix k = 2*M_PI/λ;
        fix A = ι/(k*k);
        fix m2 = *mass < 0 ? 4*k*k / (M_PI*ι) : *mass**mass;
        fix ω = sqrt(k*k + m2);

        monitor->setInputModes({A}, {k}, {ω});

        return monitor;
    }

    Str NumericalRecipe_Ak2::SuggestFileName() const {
        const auto SEPARATOR = " ";

        StrVector params = {"Ak2", "wavelengths"};
        // if(*BCSelection == 1) params.emplace_back("harmonic");

        auto strParams = Interface->ToString(params, SEPARATOR);
        return KGRtoR::FKGRtoR_Recipe::SuggestFileName() + SEPARATOR + strParams;
    }

    TPointer<Base::FunctionT<DevFloat, DevFloat>> NumericalRecipe_Ak2::GetNonHomogenousTerm() {
        if(*driving_force && squareWave == nullptr) squareWave = Slab::New<Modes::SquareWave>(1);

        return squareWave;
    }

} // Modes