#ifndef SIMULATION_H
#define SIMULATION_H

#include "Stepper.h"
#include "OutputManager.h"

#include "Core/Controller/CommandLine/CLInterface.h"
#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"
#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"
#include "Core/Controller/CommandLine/CLInterfaceOwner.h"

namespace Slab::Math::Base {

    class NumericalRecipe : public Core::CLInterfaceOwner {
    protected:
        Pointer<NumericConfig> numeric_config;

        Str name;

        explicit NumericalRecipe(const Pointer<NumericConfig>& numeric_config, const Str& name, const Str& generalDescription, bool doRegister = false);

    public:
        virtual auto buildOutputManager()         -> Pointer<OutputManager> = 0;
        // virtual auto buildSolver()                -> Pointer<Base::LinearStepSolver> = 0;
        virtual auto buildStepper()               -> Pointer<Stepper> = 0;

        virtual auto suggestFileName()      const -> Str;

        auto getNumericConfig()             const -> const Pointer<NumericConfig> &;
    };

    DefinePointers(NumericalRecipe)

}



#endif // SIMULATION_H
