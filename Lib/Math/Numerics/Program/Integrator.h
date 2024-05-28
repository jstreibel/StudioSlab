#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include "Math/Numerics/SimConfig/NumericConfig.h"

#include "Math/Numerics/Output/OutputManager.h"

#include "Math/DifferentialEquations/Solver.h"

#include "Math/DifferentialEquations/BoundaryConditions.h"

#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Method/Method-RK4.h"
#include "Math/Numerics/Method/Method-MCBase.h"

#include "Core/Backend/Modules/TaskManager/Task.h"
#include "Core/Tools/BenchmarkHistogram.h"

const auto FORCE_INITIAL_OUTPUT = true;

namespace Slab::Math {

    class NumericalIntegration : public Task {
        Base::VoidBuilder &simBuilder;
        Stepper *stepper;
        OutputManager *outputManager;

        Real dt;
        const UInt totalSteps;
        UInt stepsConcluded;

        bool forceStopFlag = false;

        BenchmarkHistogram simTimeHistogram;

        void output(bool force = false);

        OutputPacket getOutputInfo();

        auto _cycle(size_t nCycles) -> bool;

        auto _cycleUntilOutputOrFinish() -> bool;

    public:
        explicit NumericalIntegration(Base::VoidBuilder &simBuilder)
                : simBuilder(simBuilder), stepper(simBuilder.buildStepper()),
                  outputManager(simBuilder.buildOutputManager()),
                  dt(simBuilder.getNumericParams().getdt()),
                  totalSteps(simBuilder.getNumericParams().getn()),
                  stepsConcluded(0) {
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

        ~NumericalIntegration() override;

        bool run() override;

        void forceStop() override;

        auto getSteps() const -> size_t;

        auto getSimulationTime() const -> Real;

        auto getHistogram() const -> const BenchmarkHistogram &;

    };


}

#endif // def INTEGRATOR_H
