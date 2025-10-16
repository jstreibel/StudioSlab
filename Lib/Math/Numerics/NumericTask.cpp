#include "NumericTask.h"

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif

namespace Slab::Math {

    using Core::Log;

    FNumericTask::FNumericTask(const TPointer <Base::FNumericalRecipe> &recipe, const bool pre_init)
    : FTask("Numeric Integration")
    , Recipe(recipe)
    , TotalSteps(0)
    , StepsConcluded(0)
    , Stepper(nullptr) {
        if (pre_init) Init();
    }

    FNumericTask::~FNumericTask() {
        Log::Note() << "Avg. integration time: " << BenchmarkData << Log::Flush;
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
                Log::Error() << "Couldn't set realtime scheduling: " << std::strerror(errno) << Log::Flush;
            } else {
                Log::Info() << "Program running with realtime priority." << Log::Flush;
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

    bool FNumericTask::CycleUntilOutputOrFinish() {
        const size_t nCyclesToNextOutput = OutputManager->ComputeNStepsToNextOutput(StepsConcluded);

        if (nCyclesToNextOutput > 50000) {
            Log::WarningImportant() << "Huge nCyclesToNextOutput: " << nCyclesToNextOutput << Log::Flush;
        }

        if (nCyclesToNextOutput == 0) {
            return false;
        }

        return Cycle(nCyclesToNextOutput);
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
        return static_cast<float>(StepsConcluded) / static_cast<float>(TotalSteps);
    }

    Core::ETaskStatus FNumericTask::Run() {
        if (!IsInitialized()) Init();

        Recipe->setupForCurrentThread();

        const size_t n = TotalSteps;

        while (!forceStopFlag && StepsConcluded < n && CycleUntilOutputOrFinish()) { }

        if(forceStopFlag)                                       return Core::TaskAborted;

        // Para cumprir com os steps quebrados faltantes:
        if (StepsConcluded < n) if(!Cycle(n - StepsConcluded)) return Core::TaskError;

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
