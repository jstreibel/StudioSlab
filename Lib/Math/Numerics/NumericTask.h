#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "Math/Numerics/Output/OutputManager.h"

#include "Math/Numerics/Solver/Solver.h"

#include "Math/Numerics/Solver/BoundaryConditions.h"

#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Method/RungeKutta4.h"
#include "Math/Numerics/Method/Method-MCBase.h"

#include "Core/Backend/Modules/TaskManager/Task.h"
#include "Core/Tools/BenchmarkData.h"

const auto FORCE_INITIAL_OUTPUT = true;

namespace Slab::Math {

    class NumericTask : public Core::Task {
        Base::NumericalRecipe &numericalRecipe;
        Stepper_ptr stepper;
        Pointer<OutputManager> outputManager;

        const UInt totalSteps;
        UInt stepsConcluded;

        std::atomic<bool> forceStopFlag = false;

        Core::BenchmarkData benchmarkData;

        void output(bool force = false);

        OutputPacket getOutputInfo();

        auto _cycle(size_t nCycles) -> bool;

        auto _cycleUntilOutputOrFinish() -> bool;

    protected:
        Core::TaskStatus run() override;
    public:
        explicit NumericTask(Base::NumericalRecipe &recipe);

        ~NumericTask() override;

        void abort() override;

        auto getSteps() const -> size_t;

        auto getBenchmarkData() const -> const Core::BenchmarkData &;

    };


}

#endif // def INTEGRATOR_H
