//
// Created by joao on 9/20/22.
//

#include "R2ToRBCInterface.h"

R2toRBCInterface::R2toRBCInterface(String generalDescription, String name,
                                   OutputStructureBuilderBase *outputStructureBuilder, bool selfRegister)
                                   : Base::BCInterface(generalDescription, outputStructureBuilder,
                                                       "R2toR-" + name, selfRegister)
{
    addSubInterface(outputStructureBuilder);
}

auto R2toRBCInterface::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->toString();
    return outputStructureBuilder->build(outputFileName);
}


