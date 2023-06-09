//
// Created by joao on 9/20/22.
//

#ifndef STUDIOSLAB_R2TOR_SIMULATIONBUILDER_H
#define STUDIOSLAB_R2TOR_SIMULATIONBUILDER_H


#include "Mappings/SimulationBuilder.h"
#include "Mappings/R2toR/View/OutputStructureBuilderR2ToR.h"

namespace R2toR {
    class SimulationBuilder : public Base::SimulationBuilder {
        String name;

    public:
        explicit SimulationBuilder(String generalDescription,
                                   String name = "",
                                   Numerics::OutputSystem::Builder *outputStructureBuilder = new OutputSystem::Builder,
                                   bool selfRegister=false);

        auto buildOutputManager() -> OutputManager * override;

        auto registerAllocator() const -> void override;


        bool operator==(const Interface &rhs) const override;

        bool operator==(String val) const override;
    };
}


#endif //STUDIOSLAB_R2TOR_SIMULATIONBUILDER_H
