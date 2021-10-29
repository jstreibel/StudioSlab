//
// Created by joao on 10/17/21.
//

#include <Studios/Controller/Console/CLInterfaceSelector.h>
#include "BCInterface.h"

Base::BCInterface::BCInterface(String generalDescription, OutputStructureBuilderBase* osb,  bool selfRegister)
 : Interface(generalDescription, true), outputStructureBuilder(osb)
{
    if(selfRegister) CLInterfaceSelector::getInstance().registerCandidate(this);
}

auto Base::BCInterface::getOutputStructureBuilder() -> Interface * {
    return outputStructureBuilder;
}

