#include "NumericTask.h"

#include <algorithm>

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif

namespace Slab::Math {

    using Core::FLog;
    namespace {
        constexpr size_t MaxIntegrationBatchSteps = 2048;
    }

    FNumericTask::FNumericTask(const TPointer <Base::FNumericalRecipe> &recipe, const bool pre_init)
    : FTask("Numeric Integration")
    , Recipe(recipe)
    , TotalSteps(0)
    , StepsConcluded(0)
    , Stepper(nullptr) {
        if (pre_init) Init();
    }

    FNumericTask::~FNumericTask() {
        if (BenchmarkData == nullptr) return;

        FLog::Note() << "Avg. integration time: " << *BenchmarkData << FLog::Flush;
    }

    void FNumericTask::Init() {
        if (IsInitialized()) { throw Exception("Numeric task already initialized"); }

        TotalSteps = Recipe->GetNumericConfig()->Get_n();
        StepsConcluded = 0;
        Stepper = Recipe->BuildStepper();
        BenchmarkData = New<Core::FBenchmarkData>(TotalSteps/100);

        if (OutputManager == nullptr)
        {
            OutputManager = New<FOutputManager>(TotalSteps);
            for(const auto sockets = Recipe->BuildOutputSockets(); const auto &socket : sockets)
            {
                OutputManager->AddOutputChannel(socket);
            }
        }
        else OutputManager->SetMaxSteps(TotalSteps);

#if ATTEMP_REALTIME
        {
            // Declare a sched_param struct to hold the scheduling parameters.
            sched_param param;

            // Set the priority value in the sched_param struct.
            param.sched_priority = sched_get_priority_max(SCHED_FIFO);

            // Set the scheduling policy and priority of the current process.
            int ret = sched_setscheduler(0, SCHED_FIFO, &param);
            if (ret == -1) {
                FLog::Error() << "Couldn't set realtime scheduling: " << std::strerror(errno) << FLog::Flush;
            } else {
                FLog::Info() << "Program running with realtime priority." << FLog::Flush;
            }
        }
#endif

        this->Output(FORCE_INITIAL_OUTPUT);
    }

    auto FNumericTask::IsInitialized() const -> bool {
        return Stepper != nullptr;
    }

    FOutputPacket FNumericTask::GetOutputInfo() {
        auto currentState = Stepper->GetCurrentState();

        return {currentState, StepsConcluded};
    }

    bool FNumericTask::Cycle(size_t nCycles) {
        if(forceStopFlag) return false;

        BenchmarkData->StartMeasure();
        Stepper->Step(nCycles);
        BenchmarkData->StoreMeasure(static_cast<int>(nCycles));

        StepsConcluded += nCycles;

        fix forceOutput = StepsConcluded >= TotalSteps;

        Output(forceOutput);

        return true;
    }

    auto FNumericTask::ComputeCycleSize(const size_t remainingSteps) -> size_t {
        if (remainingSteps == 0) return 0;

        auto nCycles = std::min(remainingSteps, MaxIntegrationBatchSteps);

        if (OutputManager != nullptr) {
            const size_t nCyclesToNextOutput = OutputManager->ComputeNStepsToNextOutput(StepsConcluded);

            if (nCyclesToNextOutput == 0) {
                FLog::WarningImportant() << "Output manager requested 0 cycle size at step "
                                         << StepsConcluded << ". Falling back to integration batch." << FLog::Flush;
                return nCycles;
            }

            nCycles = std::min(nCycles, nCyclesToNextOutput);
        }

        return nCycles;
    }

    void FNumericTask::Output(const bool force) {
        FOutputPacket info = GetOutputInfo();
        OutputManager->Output(info, force);
    }

    size_t FNumericTask::GetSteps() const { return StepsConcluded; }

    auto FNumericTask::GetStepper() const -> FStepper_ptr
    {
        return Stepper;
    }

    const Core::FBenchmarkData &FNumericTask::GetBenchmarkData() const {
        return *BenchmarkData;
    }

    float FNumericTask::GetProgress() const
    {
        if (TotalSteps == 0) return 1.0f;

        return static_cast<float>(StepsConcluded) / static_cast<float>(TotalSteps);
    }

    Core::ETaskStatus FNumericTask::Run() {
        if (!IsInitialized()) Init();

        Recipe->setupForCurrentThread();

        const size_t n = TotalSteps;

        while (!forceStopFlag && StepsConcluded < n) {
            const size_t remainingSteps = n - StepsConcluded;
            const size_t nCycles = ComputeCycleSize(remainingSteps);

            if (nCycles == 0) return Core::TaskError;
            if (!Cycle(nCycles)) return forceStopFlag ? Core::TaskAborted : Core::TaskError;
        }

        if (forceStopFlag) return Core::TaskAborted;

        OutputManager->NotifyIntegrationFinished(GetOutputInfo());

        return Core::TaskSuccess;
    }

    void FNumericTask::Abort() {
        forceStopFlag = true;
    }

    void FNumericTask::SetOutputManager(const TPointer<FOutputManager>& CustomManager)
    {
        if(CustomManager == nullptr) return;
        OutputManager = CustomManager;
    }

    TPointer<const Base::FNumericalRecipe> FNumericTask::GetRecipe() const
    {
        return Recipe;
    }

    TPointer<Base::FNumericalRecipe> FNumericTask::GetRecipe()
    {
        return Recipe;
    }
}
