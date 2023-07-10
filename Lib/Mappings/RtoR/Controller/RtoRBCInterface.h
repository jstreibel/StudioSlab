//
// Created by joao on 10/18/21.
//

#ifndef FIELDS_RTORBCINTERFACE_H
#define FIELDS_RTORBCINTERFACE_H

#include "Mappings/RtoR/View/OutputStructureBuilderRtoR.h"

#include "Mappings/SimulationBuilder.h"

class RtoRBCInterface : public Base::SimulationBuilder {
public:
    typedef Numerics::OutputSystem::Builder::Ptr BuilderBasePtr;

    explicit RtoRBCInterface(Str name,
                             Str generalDescription);

    explicit RtoRBCInterface(Str name,
                             Str generalDescription,
                             BuilderBasePtr outputStructureBuilder);

    auto buildOutputManager() -> OutputManager * override;


};


#endif //FIELDS_RTORBCINTERFACE_H
