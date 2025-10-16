#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "OutputManager.h"

#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"

#include "Math/Numerics/ODE/Solver/BoundaryConditions.h"

#include "Stepper.h"
#include "NumericalRecipe.h"

#include "Core/Backend/Modules/TaskManager/Task.h"
#include "Core/Tools/BenchmarkData.h"

constexpr auto FORCE_INITIAL_OUTPUT = true;

namespace Slab::Math {

    class FNumericTask final : public Core::FTask {
        TPointer<Base::FNumericalRecipe> Recipe;

        UInt TotalSteps;
        UInt StepsConcluded;

        FStepper_ptr Stepper;
        TPointer<FOutputManager> OutputManager;

        std::atomic<bool> forceStopFlag = false;

        TPointer<Core::FBenchmarkData> BenchmarkData;

        void Output(bool force = false);

        FOutputPacket GetOutputInfo();

        auto Cycle(size_t nCycles) -> bool;

        auto CycleUntilOutputOrFinish() -> bool;

    protected:
        Core::ETaskStatus Run() override;
    public:
        explicit FNumericTask(const TPointer<Base::FNumericalRecipe> &recipe, bool pre_init=true);

        ~FNumericTask() override;

        void Init();
        [[nodiscard]] auto IsInitialized() const -> bool;

        void Abort() override;

        /**
         * Option to provide a custom FOutputManager. Means the NumericTask will not call
         * FNumericalRecipe::BuildOutputChannels. Notice that the constructor's pre_init parameter ideally is
         * set to 'false', if you'll provide a CustomManager. Does nothing is CustomManager is nullptr.
         * @param CustomManager the custom manager to replace the default one, provided by FNumericalRecipe.
         */
        void SetOutputManager(const TPointer<FOutputManager> &CustomManager);

        [[nodiscard]] TPointer<const Base::FNumericalRecipe> GetRecipe() const;
        TPointer<Base::FNumericalRecipe> GetRecipe();

        [[nodiscard]] auto GetSteps() const -> size_t;

        [[nodiscard]] auto GetStepper() const -> FStepper_ptr;

        [[nodiscard]] auto GetBenchmarkData() const -> const Core::FBenchmarkData &;
        [[nodiscard]] float GetProgress() const;
    };


}

#endif // def INTEGRATOR_H
