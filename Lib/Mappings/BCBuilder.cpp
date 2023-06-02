//
// Created by joao on 10/17/21.
//

#include "Base/Controller/Interface/InterfaceSelector.h"
#include "BCBuilder.h"

Base::BCBuilder::BCBuilder(String generalDescription, Numerics::OutputSystem::StructureBuilder* osb,
                           String prefix, bool selfRegister)
 : Interface(generalDescription, true), outputStructureBuilder(osb), prefix(prefix)
{
    if(selfRegister) InterfaceSelector::getInstance().registerOption(this);
}

auto Base::BCBuilder::getOutputStructureBuilder() -> Numerics::OutputSystem::StructureBuilder * {
    return outputStructureBuilder;
}

String Base::BCBuilder::toString() const {
    auto strParams = Interface::toString();

    auto str = prefix + "-" + strParams;

    return str;
}

