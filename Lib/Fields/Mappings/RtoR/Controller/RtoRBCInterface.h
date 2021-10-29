//
// Created by joao on 10/18/21.
//

#ifndef FIELDS_RTORBCINTERFACE_H
#define FIELDS_RTORBCINTERFACE_H

#include "../View/OutputStructureBuilderRtoR.h"

#include <Fields/Mappings/BCInterface.h>

class RtoRBCInterface : public Base::BCInterface {
public:
    explicit RtoRBCInterface(String generalDescription,
                             OutputStructureBuilderBase *outputStructureBuilder =  new OutputStructureBuilderRtoR,
                             bool selfRegister=false);

    auto buildOutputManager() -> OutputManager * override;


};


#endif //FIELDS_RTORBCINTERFACE_H