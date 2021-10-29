//
// Created by joao on 10/18/21.
//

#include "RtoRBCInterface.h"

#include <utility>


RtoRBCInterface::RtoRBCInterface(String generalDescription,
                                 OutputStructureBuilderBase *outputStructureBuilder,
                                 bool selfRegister)
    : BCInterface(std::move(generalDescription), outputStructureBuilder, selfRegister)
{

}

auto RtoRBCInterface::buildOutputManager() -> OutputManager * {
    return outputStructureBuilder->build();
}

