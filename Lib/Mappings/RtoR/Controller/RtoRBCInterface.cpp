//
// Created by joao on 10/18/21.
//

#include "RtoRBCInterface.h"

#include <utility>


RtoRBCInterface::RtoRBCInterface(String generalDescription,
                                 String name,
                                 OutputStructureBuilderBase *outputStructureBuilder,
                                 bool selfRegister)
    : BCBuilder(std::move(generalDescription), outputStructureBuilder,
                  "RtoR-" + name, selfRegister)
{
    addSubInterface(outputStructureBuilder);
}

auto RtoRBCInterface::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->toString();
    return outputStructureBuilder->build(outputFileName);
}

