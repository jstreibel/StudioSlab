//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H

#include "Apps/Simulations/OutputStructureBuilders/OutputStructureBuilderBase.h"

class OutputStructureBuilderR2toR : public OutputStructureBuilderBase {
public:
    auto build() -> OutputManager* override;


};


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H
