//
// Created by joao on 10/17/21.
//

#include "../../Core/Controller/CommandLineInterfaceSelector.h"
#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"
#include "NumericalRecipe.h"

#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, EqStateType>(dPhi); \
break;

#define DONT_REGISTER false


namespace Slab::Math::Base {


    FNumericalRecipe::FNumericalRecipe(const TPointer<FNumericConfig>& numeric_config, const Str &name,
                                     const Str& generalDescription, bool doRegister)
    : FCommandLineInterfaceOwner(name, 100, DONT_REGISTER)
    , NumericConfig(numeric_config)
    , Name(name)
    {
        Interface->AddSubInterface(numeric_config->GetInterface());
        Interface->SetGeneralDescription(generalDescription);

        if (doRegister) {
            Core::RegisterCLInterface(Interface);
        }

        Core::Log::Status() << "SimulationBuilder '" << Interface->GetName() << "': \""
                      << Interface->GetGeneralDescription() << "\" instantiated." << Core::Log::Flush;
    }

    auto FNumericalRecipe::GetNumericConfig() const -> const TPointer<FNumericConfig> & {
        return NumericConfig;
    }

    Str FNumericalRecipe::SuggestFileName() const {
        return Name + " " + NumericConfig->to_string();
    }

}