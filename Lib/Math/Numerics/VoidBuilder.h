#ifndef SIMULATION_H
#define SIMULATION_H

#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Output/OutputManager.h"

#include "Core/Controller/Interface/Interface.h"
#include "Math/Numerics/SimConfig/SimulationConfig.h"
#include "Math/DifferentialEquations/Solver.h"

namespace Slab::Math::Base {

    class VoidBuilder : public InterfaceOwner {
    protected:
        SimulationConfig simulationConfig;

        Str prefix;

        explicit VoidBuilder(const Str& name, Str generalDescription, bool doRegister = false);

    public:
        typedef std::shared_ptr<VoidBuilder> Ptr;

        virtual auto buildOutputManager()         -> OutputManager * = 0;
        virtual auto buildEquationSolver()        -> Solver_ptr = 0;
        virtual auto buildStepper()               -> Stepper* = 0;

        virtual auto suggestFileName()      const -> Str;

        auto getNumericParams()             const -> const NumericConfig &;
    };

    DefinePointer(VoidBuilder)

}



#endif // SIMULATION_H
