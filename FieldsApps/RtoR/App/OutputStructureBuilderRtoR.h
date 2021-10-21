//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H

#include <Apps/Backend/Backend.h>
#include "Apps/Simulations/OutputStructureBuilders/OutputStructureBuilderBase.h"

class OutputStructureBuilderRtoR : public OutputStructureBuilderBase {
public:
    OutputStructureBuilderRtoR();

    auto build() -> OutputManager * override;
};


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H
