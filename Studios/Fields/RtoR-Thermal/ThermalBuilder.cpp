//
// Created by joao on 8/4/23.
//

#include "ThermalBuilder.h"

#include <memory>
#include "Models/KleinGordon/KG-LangevinSolver.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRStatisticalMonitor.h"
#include "../RtoR-Modes/Sim/Monitor.h"

// Don't touch:
#define DONT_SELF_REGISTER false
#define SHORT_NAME false
#define LONG_NAME true

namespace Studios::Fields::RtoRThermal {

    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister)
    : KGRtoRBuilder(name, generalDescription, DONT_SELF_REGISTER) {
        interface->addParameters({&temperature, &dissipation, &transientGuess});

        setLaplacianPeriodicBC();

        if(doRegister) registerToManager();
    }

    auto Builder::buildSolver() -> Pointer<Base::LinearStepSolver> {
        auto bc = getBoundary();
        auto pot = getPotential();

        auto config = DynamicPointerCast<KGNumericConfig>(getNumericConfig());

        auto solver = New<Slab::Models::KGRtoR::KGLangevinSolver>(bc, pot);
        solver->setTemperature(*temperature);
        solver->setDissipationCoefficient(*dissipation);

        return solver;
    }

    Str Builder::suggestFileName() const {
        auto str = NumericalRecipe::suggestFileName();

        auto extra1 = interface->toString({"T", "k"}, " ", SHORT_NAME);
        auto extra2 = interface->toString({"E", "n"}, " ", LONG_NAME);

        return str + " " + extra1 + " " + extra2;
    }


}
