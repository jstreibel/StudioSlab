//
// Created by joao on 10/17/21.
//

#include "Core/Controller/CommandLine/CommandLineInterfaceSelector.h"
#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"
#include "NumericalRecipe.h"

#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, EqStateType>(dPhi); \
break;

#define DONT_REGISTER false


namespace Slab::Math::Base {


    NumericalRecipe::NumericalRecipe(const TPointer<NumericConfig>& numeric_config, const Str &name,
                                     const Str& generalDescription, bool doRegister)
            : FCommandLineInterfaceOwner(name, 100, DONT_REGISTER), numeric_config(numeric_config), name(name) {
        Interface->AddSubInterface(numeric_config->GetInterface());

        if (doRegister) {
            Core::RegisterCLInterface(Interface);
        }

        Core::Log::Status() << "SimulationBuilder '" << Interface->GetName() << "': \""
                      << Interface->GetGeneralDescription() << "\" instantiated." << Core::Log::Flush;
    }

    auto NumericalRecipe::getNumericConfig() const -> const TPointer<NumericConfig> & {
        return numeric_config;
    }

    Str NumericalRecipe::SuggestFileName() const {
        return name + " " + numeric_config->to_string();
    }

}