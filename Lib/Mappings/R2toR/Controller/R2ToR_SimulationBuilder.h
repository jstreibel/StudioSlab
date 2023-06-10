//
// Created by joao on 9/20/22.
//

#ifndef STUDIOSLAB_R2TOR_SIMULATIONBUILDER_H
#define STUDIOSLAB_R2TOR_SIMULATIONBUILDER_H


#include "Mappings/SimulationBuilder.h"
#include "Mappings/R2toR/View/OutputStructureBuilderR2ToR.h"

namespace R2toR {
    typedef Numerics::OutputSystem::Builder::Ptr BuilderBasePtr;

    class SimulationBuilder : public Base::SimulationBuilder {

    public:
        explicit SimulationBuilder(String name,
                          BuilderBasePtr outputStructureBuilder = BuilderBasePtr(new OutputSystem::Builder));
        auto buildOutputManager()         -> OutputManager *  override;
        auto registerAllocator ()   const -> void             override;

    };
}


#endif //STUDIOSLAB_R2TOR_SIMULATIONBUILDER_H
