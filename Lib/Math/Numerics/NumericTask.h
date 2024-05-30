#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "Math/Numerics/SimConfig/NumericConfig.h"

#include "Math/Numerics/Output/OutputManager.h"

#include "Math/DifferentialEquations/Solver.h"

#include "Math/DifferentialEquations/BoundaryConditions.h"

#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Method/RungeKutta4.h"
#include "Math/Numerics/Method/Method-MCBase.h"

#include "Core/Backend/Modules/TaskManager/Task.h"
#include "Core/Tools/BenchmarkData.h"

const auto FORCE_INITIAL_OUTPUT = true;

namespace Slab::Math {

    class NumericTask : public Task {
        Base::NumericalRecipe &numericalRecipe;
        Stepper_ptr stepper;
        Pointer<OutputManager> outputManager;

        Real dt;
        const UInt totalSteps;
        UInt stepsConcluded;

        bool forceStopFlag = false;

        BenchmarkData benchmarkData;

        void output(bool force = false);

        OutputPacket getOutputInfo();

        auto _cycle(size_t nCycles) -> bool;

        auto _cycleUntilOutputOrFinish() -> bool;

    public:
        explicit NumericTask(Base::NumericalRecipe &recipe)
                : numericalRecipe(recipe), stepper(recipe.buildStepper()),
                  outputManager(recipe.buildOutputManager()),
                  dt(recipe.getNumericParams().getdt()),
                  totalSteps(recipe.getNumericParams().getn()),
                  stepsConcluded(0),
                  benchmarkData(recipe.getNumericParams().getn()/100){
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

        ~NumericTask() override;

        bool run() override;

        void forceStop() override;

        auto getSteps() const -> size_t;

        auto getSimulationTime() const -> Real;

        auto getBenchmarkData() const -> const BenchmarkData &;

    };


}

#endif // def INTEGRATOR_H
