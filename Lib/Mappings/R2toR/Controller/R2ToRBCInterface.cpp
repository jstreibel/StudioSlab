//
// Created by joao on 9/20/22.
//

#include "R2ToRBCInterface.h"

R2toRBCInterface::R2toRBCInterface(String generalDescription, String name,
                                   Numerics::OutputSystem::StructureBuilder *outputStructureBuilder, bool selfRegister)
                                   : Base::BCBuilder(generalDescription, outputStructureBuilder,
                                                       "R2toR-" + name, selfRegister), name(name)
{
    addSubInterface(outputStructureBuilder);
}

auto R2toRBCInterface::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->toString();
    return outputStructureBuilder->build(outputFileName);
}

bool R2toRBCInterface::operator==(const Interface &rhs) const {
    auto other = dynamic_cast<const R2toRBCInterface*>(&rhs);

    if(other == nullptr) return Interface::operator==(rhs);

    return this->name == other->name;
}

bool R2toRBCInterface::operator==(String val) const {
    return name == val;
}


