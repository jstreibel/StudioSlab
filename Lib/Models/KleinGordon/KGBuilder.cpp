//
// Created by joao on 20/09/23.
//

#include "KGBuilder.h"

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Math/Numerics/Method/RungeKutta4.h"

namespace Slab::Models {

    KGBuilder::KGBuilder(const Str &name, Str generalDescription, bool doRegister)
            : NumericalRecipe(name, std::move(generalDescription), DONT_REGISTER) {

        interface->addParameters({&noHistoryToFile,
                                  &outputResolution,
                                  &VisualMonitor,
                                  &VisualMonitor_startPaused,
                                  &takeSnapshot,
                                  &takeDFTSnapshot,
                                         /*, &snapshotTime, */ });

        if (doRegister) InterfaceManager::getInstance().registerInterface(interface);
    }

    auto KGBuilder::buildStepper() -> Pointer<Stepper> {
        auto solver = buildEquationSolver();

        return New <RungeKutta4> (solver);
    }


    void KGBuilder::notifyAllCLArgsSetupFinished() {
        auto nThreads = NumericalRecipe::simulationConfig.dev.get_nThreads();
        auto N = NumericalRecipe::simulationConfig.numericConfig.getN();
        if (N % nThreads != 0)
            throw Exception("Bad assertion N%nThreads. Expected 0 got "
                            + ToStr(N % nThreads) + ".");

        InterfaceListener::notifyAllCLArgsSetupFinished();
    }


}