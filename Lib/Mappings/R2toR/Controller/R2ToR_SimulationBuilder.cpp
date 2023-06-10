//
// Created by joao on 9/20/22.
//

#include "R2ToR_SimulationBuilder.h"
#include "Mappings/R2toR/Core/R2toR_Allocator.h"

R2toR::SimulationBuilder::SimulationBuilder(String name, BuilderBasePtr outputStructureBuilder)
    : Base::SimulationBuilder(name, outputStructureBuilder, "R2toR-" + name)
{
    interface->addSubInterface(outputStructureBuilder->getInterface());
}


auto R2toR::SimulationBuilder::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->toString();
    return outputSystemBuilder->build(outputFileName);
}

void R2toR::SimulationBuilder::registerAllocator() const {
    Core::BasicAllocator::Choose();
}


