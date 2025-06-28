#ifndef SIMULATION_H
#define SIMULATION_H

#include "Stepper.h"
#include "Math/Numerics/Socket.h"

#include "Core/Controller/CommandLine/CommandLineInterface.h"
#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"
#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"
#include "Core/Controller/CommandLine/CommandLineInterfaceOwner.h"

namespace Slab::Math::Base {

    using OutputSockets = Vector<Pointer<Socket>>;

    class NumericalRecipe : public Core::FCommandLineInterfaceOwner {
    protected:
        Pointer<NumericConfig> numeric_config;

        Str name;

        explicit NumericalRecipe(const Pointer<NumericConfig>& numeric_config,
                                 const Str& name,
                                 const Str& generalDescription, bool doRegister = false);

    public:
        /**
         * This is mostly used because GPU needs to be set up from the thread it is operated by.
         */
        virtual void setupForCurrentThread() { };

        virtual auto buildOutputSockets()         -> OutputSockets = 0;
        virtual auto buildStepper()               -> Pointer<Stepper> = 0;

        [[nodiscard]] virtual auto suggestFileName()      const -> Str;

        auto getNumericConfig()             const -> const Pointer<NumericConfig> &;
    };

    DefinePointers(NumericalRecipe)

}



#endif // SIMULATION_H
