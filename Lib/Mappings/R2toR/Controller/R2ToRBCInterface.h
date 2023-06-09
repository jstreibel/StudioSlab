//
// Created by joao on 9/20/22.
//

#ifndef STUDIOSLAB_R2TORBCINTERFACE_H
#define STUDIOSLAB_R2TORBCINTERFACE_H


#include "Mappings/SimulationBuilder.h"
#include "Mappings/R2toR/View/OutputStructureBuilderR2ToR.h"

class R2toRBCInterface : public Base::SimulationBuilder {
    String name;
public:
    explicit R2toRBCInterface(String generalDescription,
                              String name = "",
                              Numerics::OutputSystem::StructureBuilder *outputStructureBuilder = new OutputStructureBuilderR2toR,
                              bool selfRegister=false);

    auto buildOutputManager() -> OutputManager * override;

    bool operator==(const Interface &rhs) const override;

    bool operator==(String val) const override;
};


#endif //STUDIOSLAB_R2TORBCINTERFACE_H
