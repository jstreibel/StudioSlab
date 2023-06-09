//
// Created by joao on 9/20/22.
//

#include "R2ToR_SimulationBuilder.h"
#include "Mappings/R2toR/Core/R2toR_Allocator.h"

R2toR::SimulationBuilder::SimulationBuilder(String generalDescription, String name, BuilderBasePtr outputStructureBuilder,
                                            bool selfRegister)
    : Base::SimulationBuilder(generalDescription, outputStructureBuilder, "R2toR-" + name, selfRegister), name(name)
{
    addSubInterface(*outputStructureBuilder);
}


auto R2toR::SimulationBuilder::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->toString();
    return outputSystemBuilder->build(outputFileName);
}

bool R2toR::SimulationBuilder::operator==(const Interface &rhs) const {
    auto other = dynamic_cast<const R2toR::SimulationBuilder*>(&rhs);

    if(other == nullptr) return Interface::operator==(rhs);

    return this->name == other->name;
}

bool R2toR::SimulationBuilder::operator==(String val) const {
    return name == val;
}

void R2toR::SimulationBuilder::registerAllocator() const {
    Core::BasicAllocator::Choose();
}


