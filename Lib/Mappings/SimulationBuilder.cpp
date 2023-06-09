//
// Created by joao on 10/17/21.
//

#include "Base/Controller/Interface/InterfaceSelector.h"
#include "SimulationBuilder.h"

Base::SimulationBuilder::SimulationBuilder(String generalDescription, Numerics::OutputSystem::Builder* osb,
                                           String prefix, bool selfRegister)
 : Interface(generalDescription, true), outputSystemBuilder(osb), prefix(prefix)
{
    if(selfRegister) InterfaceSelector::getInstance().registerOption(this);
}

String Base::SimulationBuilder::toString() const {
    auto strParams = Interface::toString();

    auto str = prefix + "-" + strParams;

    return str;
}

auto Base::SimulationBuilder::registerAllocator() const -> void {
    std::cout << "Warning: SimulationBuilder should be registering the Allocator." << std::endl; }

