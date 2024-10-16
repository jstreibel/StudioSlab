//
// Created by joao on 10/17/21.
//

#include "Core/Controller/CommandLine/CLInterfaceSelector.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "Core/Tools/Log.h"
#include "NumericalRecipe.h"

#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, EqStateType>(dPhi); \
break;

#define DONT_REGISTER false


namespace Slab::Math::Base {


    NumericalRecipe::NumericalRecipe(const Pointer<NumericConfig>& numeric_config, const Str &name,
                                     const Str& generalDescription, bool doRegister)
            : CLInterfaceOwner(name, 100, DONT_REGISTER), numeric_config(numeric_config), name(name) {
        interface->addSubInterface(numeric_config->getInterface());

        if (doRegister) {
            Core::CLInterfaceManager::getInstance().registerInterface(interface);
        }

        Core::Log::Status() << "SimulationBuilder '" << interface->getName() << "': \""
                      << interface->getGeneralDescription() << "\" instantiated." << Core::Log::Flush;
    }

    auto NumericalRecipe::getNumericConfig() const -> const Pointer<NumericConfig> & {
        return numeric_config;
    }

    Str NumericalRecipe::suggestFileName() const {
        return name + " " + numeric_config->to_string();
    }


}