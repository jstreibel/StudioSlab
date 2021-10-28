//
// Created by joao on 10/17/21.
//

#include <Controller/Console/CLInterfaceSelector.h>
#include "BCInterface.h"

Base::BCInterface::BCInterface(String generalDescription, OutputStructureBuilderBase* osb,  bool selfRegister)
 : Interface(generalDescription, true), outputStructureBuilder(osb)
{
    if(selfRegister) CLInterfaceSelector::getInstance().registerBCInterface(this);
}

auto Base::BCInterface::getOutputStructureBuilder() -> Interface * {
    return outputStructureBuilder;
}

