//
// Created by joao on 9/20/22.
//

#ifndef STUDIOSLAB_R2TORBCINTERFACE_H
#define STUDIOSLAB_R2TORBCINTERFACE_H


#include "Fields/Mappings/BCInterface.h"
#include "Fields/Mappings/R2toR/View/OutputStructureBuilderR2ToR.h"

class R2toRBCInterface : public Base::BCInterface {
public:
    explicit R2toRBCInterface(String generalDescription,
                              String name = "",
                              OutputStructureBuilderBase *outputStructureBuilder = new OutputStructureBuilderR2toR,
                              bool selfRegister=false);

    auto buildOutputManager() -> OutputManager * override;
};


#endif //STUDIOSLAB_R2TORBCINTERFACE_H
