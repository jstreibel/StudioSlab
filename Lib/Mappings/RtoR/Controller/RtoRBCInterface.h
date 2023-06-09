//
// Created by joao on 10/18/21.
//

#ifndef FIELDS_RTORBCINTERFACE_H
#define FIELDS_RTORBCINTERFACE_H

#include "Mappings/RtoR/View/OutputStructureBuilderRtoR.h"

#include "Mappings/SimulationBuilder.h"

class RtoRBCInterface : public Base::SimulationBuilder {
public:
    explicit RtoRBCInterface(String generalDescription,
                             String name = "",
                             OutputStructureBuilderBase *outputStructureBuilder =  new OutputStructureBuilderRtoR,
                             bool selfRegister=false);

    auto buildOutputManager() -> OutputManager * override;


};


#endif //FIELDS_RTORBCINTERFACE_H
