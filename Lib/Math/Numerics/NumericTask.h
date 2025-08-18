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
        TPointer<Base::FNumericalRecipe> Recipe;

        UInt TotalSteps;
        UInt StepsConcluded;

        Stepper_ptr stepper;
        TPointer<FOutputManager> OutputManager;

        std::atomic<bool> forceStopFlag = false;

        TPointer<Core::BenchmarkData> BenchmarkData;

        void output(bool force = false);

        OutputPacket getOutputInfo();

        auto _cycle(size_t nCycles) -> bool;

        auto _cycleUntilOutputOrFinish() -> bool;

    protected:
        Core::ETaskStatus Run() override;
    public:
        explicit NumericTask(const TPointer<Base::FNumericalRecipe> &recipe, bool pre_init=true);

        ~NumericTask() override;

        void Init();
        [[nodiscard]] auto isInitialized() const -> bool;

        void Abort() override;

        /**
         * Option to provide a custom FOutputManager. Means the NumericTask will not call
         * FNumericalRecipe::BuildOutputChannels. Notice that the constructor's pre_init parameter ideally is
         * set to 'false', if you'll provide a CustomManager.
         * @param CustomManager the custom manager to replace the default one, provided by FNumericalRecipe.
         */
        void SetOutputManager(const TPointer<FOutputManager> &CustomManager);

        [[nodiscard]] TPointer<const Base::FNumericalRecipe> GetRecipe() const;
        TPointer<Base::FNumericalRecipe> GetRecipe();

        [[nodiscard]] auto getSteps() const -> size_t;

        [[nodiscard]] auto getBenchmarkData() const -> const Core::BenchmarkData &;
        [[nodiscard]] float GetProgress() const;
    };


}

#endif // def INTEGRATOR_H
