#ifndef SIMULATION_H
#define SIMULATION_H

#include "Stepper.h"
#include "Math/Numerics/OutputChannel.h"

#include "../../Core/Controller/Interface.h"
#include "Math/Numerics/ODE/SimConfig/NumericConfig.h"
#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"
#include "../../Core/Controller/InterfaceOwner.h"

namespace Slab::Math::Base {

    using OutputSockets = Vector<TPointer<FOutputChannel>>;

    class FNumericalRecipe : public Core::FInterfaceOwner {
    protected:
        TPointer<FNumericConfig> NumericConfig;

        Str Name;

        explicit FNumericalRecipe(const TPointer<FNumericConfig>& numeric_config,
                                 const Str& name,
                                 const Str& generalDescription, bool doRegister = false);

    public:
        /**
         * This is mostly used because GPU needs to be set up from the thread it is operated by.
         */
        virtual void setupForCurrentThread() { };

        virtual auto BuildOutputSockets()         -> OutputSockets = 0;
        virtual auto BuildStepper()               -> TPointer<FStepper> = 0;

        [[nodiscard]] virtual auto SuggestFileName()      const -> Str;

        auto GetNumericConfig()             const -> const TPointer<FNumericConfig> &;
    };

    DefinePointers(FNumericalRecipe)

}



#endif // SIMULATION_H
