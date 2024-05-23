//
// Created by joao on 10/17/21.
//

#include "Core/Controller/Interface/InterfaceSelector.h"
#include "Core/Controller/Interface/InterfaceManager.h"
#include "Core/Tools/Log.h"
#include "VoidBuilder.h"
//#include "Math/Numerics/Method/Method-RK4.h"

#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, EqStateType>(dPhi); \
break;

#define DONT_REGISTER false


namespace Slab::Math {


    VoidBuilder::VoidBuilder(const Str &name, Str generalDescription, bool doRegister)
            : InterfaceOwner(name, 100, DONT_REGISTER), simulationConfig(DONT_REGISTER), prefix(name) {
        interface->addSubInterface(simulationConfig.numericConfig.getInterface());
        interface->addSubInterface(simulationConfig.dev.getInterface());

        if (doRegister) {
            InterfaceManager::getInstance().registerInterface(interface);
        }

        Log::Status() << "SimulationBuilder '" << interface->getName() << "': \""
                      << interface->getGeneralDescription() << "\" instantiated." << Log::Flush;
    }

    auto VoidBuilder::getNumericParams() const -> const NumericConfig & {
        return simulationConfig.numericConfig;
    }

    auto VoidBuilder::getDevice() const -> const DeviceConfig & {
        return simulationConfig.dev;
    }

    Str VoidBuilder::suggestFileName() const {
        const auto SEPARATOR = " ";
        auto strParams = simulationConfig.numericConfig.getInterface()->toString({"L", "N"}, SEPARATOR);

        auto str = prefix + SEPARATOR + strParams;

        return str;
    }


}