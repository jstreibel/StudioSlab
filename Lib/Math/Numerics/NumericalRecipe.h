#ifndef SIMULATION_H
#define SIMULATION_H

#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Output/OutputManager.h"

#include "Core/Controller/CommandLine/CLInterface.h"
#include "Math/Numerics/SimConfig/SimulationConfig.h"
#include "Math/Numerics/Solver/Solver.h"

namespace Slab::Math::Base {

    class NumericalRecipe : public CLInterfaceOwner {
    protected:
        SimulationConfig simulationConfig;

        Str prefix;

        explicit NumericalRecipe(const Str& name, Str generalDescription, bool doRegister = false);

    public:
        virtual auto buildOutputManager()         -> Pointer<OutputManager> = 0;
        virtual auto buildEquationSolver()        -> Solver_ptr = 0;
        virtual auto buildStepper()               -> Pointer<Stepper> = 0;

        virtual auto suggestFileName()      const -> Str;

        auto getNumericParams()             const -> const NumericConfig &;
    };

    DefinePointers(NumericalRecipe)

}



#endif // SIMULATION_H
