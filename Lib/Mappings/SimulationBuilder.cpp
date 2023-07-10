//
// Created by joao on 10/17/21.
//

#include "Base/Controller/Interface/InterfaceSelector.h"
#include "Base/Controller/Interface/InterfaceManager.h"
#include "SimulationBuilder.h"
#include "Common/Log/Log.h"

Base::SimulationBuilder::SimulationBuilder(Str name, Str generalDescription, Numerics::OutputSystem::Builder::Ptr osb)
: InterfaceOwner(name, 100, false)
, outputSystemBuilder(osb)
, prefix(name)
{
    interface->addSubInterface(osb->getInterface());
    Log::Info() << "SimulationBuilder '" << interface->getName() << "': \""
                << interface->getGeneralDescription() << "\" instantiated." << Log::Flush;
}

Str Base::SimulationBuilder::toString() const {
    auto strParams = interface->toString();

    auto str = prefix + "-" + strParams;

    return str;
}


