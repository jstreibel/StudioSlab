//
// Created by joao on 9/20/22.
//

#include "R2ToR_SimulationBuilder.h"
#include "Mappings/R2toR/Core/R2toR_Allocator.h"

R2toR::SimulationBuilder::SimulationBuilder(Str name, Str descr)
: R2toR::SimulationBuilder(name, descr, BuilderBasePtr(new OutputSystem::Builder(name, descr))) {

}

R2toR::SimulationBuilder::SimulationBuilder(Str name, Str descr, BuilderBasePtr outputStructureBuilder)
    : Base::SimulationBuilder("R2toR-" + name, descr, outputStructureBuilder)
{ }



auto R2toR::SimulationBuilder::buildOutputManager() -> OutputManager * {
    auto outputFileName = this->toString();
    return outputSystemBuilder->build(outputFileName);
}

void R2toR::SimulationBuilder::registerAllocator() const {
    Numerics::Allocator::Initialize<Core::BasicAllocator>();
}




