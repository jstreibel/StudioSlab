//
// Created by joao on 10/17/21.
//

#include <Studios/Controller/Interface/InterfaceSelector.h>
#include "BCInterface.h"

Base::BCInterface::BCInterface(String generalDescription, OutputStructureBuilderBase* osb, String prefix, bool selfRegister)
 : Interface(generalDescription, true), outputStructureBuilder(osb), prefix(prefix)
{
    if(selfRegister) InterfaceSelector::getInstance().registerOption(this);
}

auto Base::BCInterface::getOutputStructureBuilder() -> Interface * {
    return outputStructureBuilder;
}

String Base::BCInterface::toString() const {
    return prefix + "-" + Interface::toString();
}

