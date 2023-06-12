//
// Created by joao on 10/17/21.
//

#include "Base/Controller/Interface/InterfaceSelector.h"
#include "Base/Controller/Interface/InterfaceManager.h"
#include "SimulationBuilder.h"
#include "Common/Log/Log.h"

Base::SimulationBuilder::SimulationBuilder(String generalDescription, Numerics::OutputSystem::Builder::Ptr osb,
                                           String prefix)
 : InterfaceOwner(generalDescription, 100, false), outputSystemBuilder(osb), prefix(prefix){
    Log::Info() << "SimulationBuilder '" << interface->getName() << "': \""
                << interface->getGeneralDescription() << "\" instantiated." << Log::Flush;
}

String Base::SimulationBuilder::toString() const {
    auto strParams = interface->toString();

    auto str = prefix + "-" + strParams;

    return str;
}

auto Base::SimulationBuilder::registerAllocator() const -> void {
    Log::WarningImportant("SimulationBuilder is not registering the Allocator.");
}


