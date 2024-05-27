//
// Created by joao on 7/20/23.
//

#ifndef STUDIOSLAB_NUMERIC_BUILDER_H
#define STUDIOSLAB_NUMERIC_BUILDER_H

#include "Math/Numerics/SimConfig/DeviceConfig.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Output/OutputManager.h"

#include "Core/Controller/Interface/Interface.h"
#include "Core/Controller/Interface/InterfaceOwner.h"
#include "Math/DifferentialEquations/EquationSolver.h"
#include "Math/DifferentialEquations/BoundaryConditions.h"
#include "Math/Numerics/Method/Method-RK4.h"

#define BUILDER_IMPL

namespace Slab::Math::Base {

    template<typename EquationSolverType>
    class NumericalRecipe : public InterfaceOwner {
    protected:
        explicit NumericalRecipe(Str name, Str generalDescription);

        NumericConfig numericParams;
        DeviceConfig dev;
        EquationState_ptr boundaries;

    public:
        typedef std::shared_ptr<NumericalRecipe> Ptr;

        using EqSolver          = EquationSolverType;
        using EqState           = EqSolver::EqState;
        using BoundaryCondition = Base::BoundaryConditions<EqState>;

        virtual ~NumericalRecipe() {}

        virtual auto buildOutputManager()         -> OutputManager * = 0;

        virtual auto getBoundary()                -> BoundaryCondition * = 0;
        virtual auto getInitialState()            -> EqState       * = 0;
        virtual auto getEquationSolver()          -> EqSolver      * = 0;

        virtual auto getMethod()                  -> Stepper*;

        auto getNumericParams()             const -> const NumericConfig &;
        auto getDevice()                    const -> const DeviceConfig &;
        auto toString()                     const -> Str;
    };


} // Simulation


#ifdef BUILDER_IMPL

#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"
#include "Math/Numerics/Method/Method-RK4.h"

#define DONT_REGISTER false

namespace Slab::Math::Base {

    template<typename SolverType>
    NumericalRecipe<SolverType>::NumericalRecipe(Str name, Str generalDescription)
            : InterfaceOwner(name, 100, DONT_REGISTER)
            , numericParams(DONT_REGISTER)
            , dev(DONT_REGISTER)
    {
        /*interface->addParameters({&noHistoryToFile, &outputResolution,
                                  &VisualMonitor, &VisualMonitor_startPaused, &OpenGLMonitor_stepsPerIdleCall
                                         &takeSnapshot, &snapshotTime }); */

        interface->addSubInterface(numericParams.getInterface());
        interface->addSubInterface(dev.getInterface());

        Log::Info() << "SimulationBuilder '" << interface->getName() << "': \""
                    << interface->getGeneralDescription() << "\" instantiated." << Log::Flush;
    }

    template<typename SolverType>
    auto NumericalRecipe<SolverType>::getNumericParams() const -> const NumericConfig & {
        return numericParams;
    }

    template<typename SolverType>
    auto NumericalRecipe<SolverType>::getDevice() const -> const DeviceConfig & {
        return dev;
    }

    template<typename SolverType>
    auto NumericalRecipe<SolverType>::toString() const -> Str {
        auto strParams = interface->toString();

        auto str = /*prefix*/ + "-" + strParams;

        return str;
    }

    template<typename SolverType>
    auto NumericalRecipe<SolverType>::getMethod() -> Stepper * {
        auto &u_0 = *getInitialState();
        auto &solver = *getEquationSolver();

        return new StepperRK4<typename SolverType::EqState>(solver, u_0);
    }
}

#endif

#endif //STUDIOSLAB_BUILDER_H
