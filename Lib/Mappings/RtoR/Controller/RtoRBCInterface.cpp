//
// Created by joao on 10/18/21.
//

#include "RtoRBCInterface.h"

#include <utility>


RtoRBCInterface::RtoRBCInterface(Str name, Str generalDescription)
: RtoRBCInterface(name, generalDescription,  BuilderBasePtr(new OutputStructureBuilderRtoR(name, generalDescription)))
{

}

RtoRBCInterface::RtoRBCInterface(Str name, Str generalDescription,
                                 RtoRBCInterface::BuilderBasePtr outputStructureBuilder)
: SimulationBuilder("RtoR-" + name, generalDescription,  outputStructureBuilder)
{

}

auto RtoRBCInterface::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->toString();
    return outputSystemBuilder->build(outputFileName);
}


