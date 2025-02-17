//
// Created by joao on 1/09/23.
//

#include "NumericalRecipe.h"

#include <memory>

#include "BoundaryCondition.h"

#include "Math/Constants.h"

#include "Core/SlabCore.h"
#include "Monitor.h"
#include "Models/KleinGordon/KG-Solver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#define DONT_REGISTER false

namespace Modes {

    // using namespace Slab::Models;

    NumericalRecipe::NumericalRecipe(bool doRegister)
    : KGRtoR::KGRtoRBuilder("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        interface->addParameters({&BCSelection, &Ak2, &wavelengths, &driving_force});


        if(doRegister) RegisterCLInterface(interface);
    }

    Math::Base::BoundaryConditions_ptr NumericalRecipe::getBoundary() {
        auto prototype = KGRtoR::KGRtoRBuilder::newFieldState();

        fix L = DynamicPointerCast<KGNumericConfig>(getNumericConfig())->getL();
        fix λ = L / (Real)*wavelengths;;
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
            if(getNonHomogenous() == nullptr) getNonHomogenous();

            return New <Modes::DrivenBC> (prototype, squareWave);
        }

        throw Exception(Str("Unknown initial condition ") + ToStr(*BCSelection));
    }

    void NumericalRecipe::notifyCLArgsSetupFinished() {
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

    void *NumericalRecipe::buildOpenGLOutput() {
        auto config = DynamicPointerCast<KGNumericConfig>(getNumericConfig());


        // fix amp = (*A) * 1.1;
        auto monitor = new Modes::Monitor(config, *(KGRtoR::KGEnergy*)getHamiltonian(), "Modes monitor");

        fix L = config->getL();
        fix λ = L / *wavelengths;
        fix ι = *Ak2;
        fix k = 2*M_PI/λ;
        fix A = ι/(k*k);
        fix m2 = *mass < 0 ? 4*k*k / (M_PI*ι) : *mass**mass;
        fix ω = sqrt(k*k + m2);

        monitor->setInputModes({A}, {k}, {ω});

        return monitor;
    }

    Str NumericalRecipe::suggestFileName() const {
        const auto SEPARATOR = " ";

        StrVector params = {"Ak2", "wavelengths"};
        // if(*BCSelection == 1) params.emplace_back("harmonic");

        auto strParams = interface->toString(params, SEPARATOR);
        return KGRtoR::KGRtoRBuilder::suggestFileName() + SEPARATOR + strParams;
    }

    Pointer<Base::FunctionT<Real, Real>> NumericalRecipe::getNonHomogenous() {
        if(*driving_force && squareWave == nullptr) squareWave = Slab::New<Modes::SquareWave>(1);

        return squareWave;
    }

} // Modes