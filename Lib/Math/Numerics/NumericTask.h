#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "OutputManager.h"

#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"

#include "Math/Numerics/ODE/Solver/BoundaryConditions.h"

#include "Stepper.h"
#include "NumericalRecipe.h"

#include "Core/Backend/Modules/TaskManager/Task.h"
#include "Core/Tools/BenchmarkData.h"

const auto FORCE_INITIAL_OUTPUT = true;

namespace Slab::Math {

    class NumericTask final : public Core::FTask {
        TPointer<Base::FNumericalRecipe> recipe;

        UInt totalSteps;
        UInt stepsConcluded;

        Stepper_ptr stepper;
        TPointer<OutputManager> outputManager;

        std::atomic<bool> forceStopFlag = false;

        TPointer<Core::BenchmarkData> benchmarkData;

        void output(bool force = false);

        OutputPacket getOutputInfo();

        auto _cycle(size_t nCycles) -> bool;

        auto _cycleUntilOutputOrFinish() -> bool;

    protected:
        Core::ETaskStatus Run() override;
    public:
        explicit NumericTask(const TPointer<Base::FNumericalRecipe> &recipe, bool pre_init=true);

        ~NumericTask() override;

        void init();
        auto isInitialized() const -> bool;

        void Abort() override;

        auto getSteps() const -> size_t;

        auto getBenchmarkData() const -> const Core::BenchmarkData &;

    };


}

#endif // def INTEGRATOR_H
