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

    Signal_Ak2_Recipe::Signal_Ak2_Recipe(bool doRegister)
    : KGRtoRBuilder("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        interface->addParameters(List<CLParameter*>{&A, &omega});

        if(doRegister) RegisterCLInterface(interface);
    }

    Base::BoundaryConditions_ptr Signal_Ak2_Recipe::getBoundary() {
        auto prototype = newFieldState();

        return New <SignalBC> (prototype, *A,  *omega);
    }

    void Signal_Ak2_Recipe::notifyCLArgsSetupFinished() {
        CLInterfaceOwner::notifyCLArgsSetupFinished();

        const auto config = DynamicPointerCast<KGNumericConfig>(getNumericConfig());

        fix L = config->getL();
        fix n = config->getn();
        fix a = config->getr();

        this->setLaplacianFixedBC();

        fix ω = *omega;

        fix period = 2*Constants::pi / ω;
        fix res = period/L * n;

        Log::Info() << "Setting ang. freq ω²=k²+m² = " << ω*ω << " ==> ω = " << ω << Log::Flush;
        Log::Info() << Log::BGWhite+Log::FGBlack << "  Technical sine resolution is " << res << " steps/cycle (" << int(res*a) << " sites/linear period)  " << Log::ResetFormatting << Log::Flush;
    }

    void *Signal_Ak2_Recipe::buildOpenGLOutput() {
        auto config = DynamicPointerCast<KGNumericConfig>(getNumericConfig());


        // fix amp = (*A) * 1.1;
        auto monitor = new Modes::Monitor(config, *static_cast<KGRtoR::KGEnergy *>(getHamiltonian()), "Modes monitor");

        monitor->setInputModes({*A}, {*omega}, {*omega});

        return monitor;
    }

    Str Signal_Ak2_Recipe::suggestFileName() const {
        const auto SEPARATOR = " ";

        const StrVector params = {A.getCommandLineArgumentName(false), omega.getCommandLineArgumentName(false)};

        const auto strParams = interface->toString(params, SEPARATOR);
        return KGRtoRBuilder::suggestFileName() + SEPARATOR + strParams;
    }

} // Modes