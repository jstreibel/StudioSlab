//
// Created by joao on 10/18/21.
//

#include "RtoRBCInterface.h"

#include <utility>


RtoRBCInterface::RtoRBCInterface(Str generalDescription, Str name, BuilderBasePtr outputStructureBuilder)
: SimulationBuilder(generalDescription, outputStructureBuilder, "RtoR-" + name)
{
    interface->addSubInterface(outputStructureBuilder->getInterface());
}

auto RtoRBCInterface::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->toString();
    return outputSystemBuilder->build(outputFileName);
}

