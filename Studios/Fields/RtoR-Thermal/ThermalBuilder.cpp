//
// Created by joao on 8/4/23.
//

#include "ThermalBuilder.h"

#include <memory>
#include "Models/LangevinKleinGordon/LangevinKGSolver.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Models/KleinGordon/RtoR/Graphics/Panels/RtoRStatisticalMonitor.h"
#include "../RtoR-Modes/Sim/Monitor.h"


#define DONT_SELF_REGISTER false
#define SHORT_NAME false

namespace Studios::Fields::RtoRThermal {

    Builder::Builder(const Str &name, const Str &generalDescription, bool doRegister)
    : KGRtoRBuilder(name, generalDescription, DONT_SELF_REGISTER) {
        interface->addParameters({&temperature, &dissipation, &transientGuess});

        setLaplacianPeriodicBC();

        if(doRegister) registerToManager();
    }

    auto Builder::buildEquationSolver() -> Base::Solver_ptr {
        auto bc = getBoundary();
        auto pot = getPotential();

        auto config = DynamicPointerCast<KGNumericConfig>(getNumericConfig());

        auto solver = New<Slab::Models::KGRtoR::LangevinKGSolver>(bc, pot);
        solver->setTemperature(*temperature);
        solver->setDissipationCoefficient(*dissipation);

        return solver;
    }

    Str Builder::suggestFileName() const {
        auto str = NumericalRecipe::suggestFileName();

        auto extra = interface->toString({"T", "k"}, " ", SHORT_NAME);

        return str + " " + extra;
    }


}
