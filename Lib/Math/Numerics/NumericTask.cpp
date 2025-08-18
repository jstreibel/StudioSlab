#include "NumericTask.h"

#define ATTEMP_REALTIME false
#if ATTEMP_REALTIME
#include <sched.h>
#include <sstream>
#endif

namespace Slab::Math {

    using Core::Log;

    NumericTask::NumericTask(const TPointer <Base::FNumericalRecipe> &recipe, const bool pre_init)
    : FTask("Numeric Integration")
    , Recipe(recipe)
    , TotalSteps(0)
    , StepsConcluded(0)
    , stepper(nullptr) {
        if (pre_init) Init();
    }

    NumericTask::~NumericTask() {
        Log::Note() << "Avg. integration time: " << BenchmarkData << Log::Flush;
    }

    void NumericTask::Init() {
        if (isInitialized()) { throw Exception("Numeric task already initialized"); }

        TotalSteps = Recipe->GetNumericConfig()->getn();
        StepsConcluded = 0;
        stepper = Recipe->buildStepper();
        OutputManager = New<FOutputManager>(TotalSteps);
        BenchmarkData = New<Core::BenchmarkData>(TotalSteps/100);
        for(auto sockets = Recipe->BuildOutputSockets();
            const auto &socket : sockets)
            OutputManager->addOutputChannel(socket);

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

        this->output(FORCE_INITIAL_OUTPUT);
    }

    auto NumericTask::isInitialized() const -> bool {
        return stepper != nullptr;
    }

    OutputPacket NumericTask::getOutputInfo() {
        auto currentState = stepper->getCurrentState();

        return {currentState, StepsConcluded};
    }

    bool NumericTask::_cycle(size_t nCycles) {
        if(forceStopFlag) return false;

        BenchmarkData->StartMeasure();
        stepper->step(nCycles);
        BenchmarkData->StoreMeasure(static_cast<int>(nCycles));

        StepsConcluded += nCycles;

        fix forceOutput = StepsConcluded >= TotalSteps;

        output(forceOutput);

        return true;
    }

    bool NumericTask::_cycleUntilOutputOrFinish() {
        const size_t nCyclesToNextOutput = OutputManager->computeNStepsToNextOutput(StepsConcluded);

        if (nCyclesToNextOutput > 50000) {
            Log::WarningImportant() << "Huge nCyclesToNextOutput: " << nCyclesToNextOutput << Log::Flush;
        }

        if (nCyclesToNextOutput == 0) {
            return false;
        }

        return _cycle(nCyclesToNextOutput);
    }

    void NumericTask::output(const bool force) {
        OutputPacket info = getOutputInfo();
        OutputManager->output(info, force);
    }

    size_t NumericTask::getSteps() const { return StepsConcluded; }

    const Core::BenchmarkData &NumericTask::getBenchmarkData() const {
        return *BenchmarkData;
    }

    float NumericTask::GetProgress() const
    {
        return static_cast<float>(StepsConcluded) / static_cast<float>(TotalSteps);
    }

    Core::ETaskStatus NumericTask::Run() {
        if (!isInitialized()) Init();

        Recipe->setupForCurrentThread();

        const size_t n = TotalSteps;

        while (!forceStopFlag && StepsConcluded < n && _cycleUntilOutputOrFinish()) { }

        if(forceStopFlag)                                       return Core::TaskAborted;

        // Para cumprir com os steps quebrados faltantes:
        if (StepsConcluded < n) if(!_cycle(n - StepsConcluded)) return Core::TaskError;

        OutputManager->notifyIntegrationFinished(getOutputInfo());

        return Core::TaskSuccess;
    }

    void NumericTask::Abort() {
        forceStopFlag = true;
    }

    TPointer<const Base::FNumericalRecipe> NumericTask::GetRecipe() const
    {
        return Recipe;
    }

    TPointer<Base::FNumericalRecipe> NumericTask::GetRecipe()
    {
        return Recipe;
    }
}
