//
// Created by joao on 1/09/23.
//

#include "Signal_Ak2_Recipe.h"

#include <memory>

#include "../BoundaryCondition.h"
#include "../Monitor.h"

#include "Math/Constants.h"

#include "Core/SlabCore.h"
#include "Models/KleinGordon/KG-Solver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Trigonometric.h"

#define DONT_REGISTER false

namespace Modes {

    FSignalAk2Recipe::FSignalAk2Recipe(bool doRegister)
    : FKGRtoR_Recipe("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        Interface->AddParameters(TList<FParameter*>{&A, &omega});

        if(doRegister) RegisterCLInterface(Interface);
    }

    Base::BoundaryConditions_ptr FSignalAk2Recipe::GetBoundary() {
        auto prototype = NewFieldState();

        return New <FSignalBC> (prototype, *A,  *omega);
    }

    void FSignalAk2Recipe::NotifyInterfaceSetupIsFinished() {
        FInterfaceOwner::NotifyInterfaceSetupIsFinished();

        const auto config = DynamicPointerCast<FKGNumericConfig>(GetNumericConfig());

        fix L = config->GetL();
        fix n = config->Get_n();
        fix a = config->Get_r();

        this->SetLaplacianFixedBC();

        fix ω = *omega;

        fix period = 2*Constants::pi / ω;
        fix res = period/L * n;

        Log::Info() << "Setting ang. freq ω²=k²+m² = " << ω*ω << " ==> ω = " << ω << Log::Flush;
        Log::Info() << Log::BGWhite+Log::FGBlack << "  Technical sine resolution is " << res << " steps/cycle (" << int(res*a) << " sites/linear period)  " << Log::ResetFormatting << Log::Flush;
    }

    void *FSignalAk2Recipe::BuildOpenGLOutput() {
        auto config = DynamicPointerCast<FKGNumericConfig>(GetNumericConfig());


        // fix amp = (*A) * 1.1;
        auto monitor = new Modes::FMonitor(config, *static_cast<KGRtoR::FKGEnergy *>(getHamiltonian()), "Modes monitor");

        monitor->setInputModes({*A}, {*omega}, {*omega});

        return monitor;
    }

    Str FSignalAk2Recipe::SuggestFileName() const {
        const auto SEPARATOR = " ";

        const StrVector params = {A.GetCommandLineArgumentName(false), omega.GetCommandLineArgumentName(false)};

        const auto strParams = Interface->ToString(params, SEPARATOR);
        return FKGRtoR_Recipe::SuggestFileName() + SEPARATOR + strParams;
    }

} // Modes
