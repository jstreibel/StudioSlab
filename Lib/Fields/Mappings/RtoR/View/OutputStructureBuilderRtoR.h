//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H

#include <Studios/CoreMath/Numerics/Output/OutputStructureBuilderBase.h>

class OutputStructureBuilderRtoR : public OutputStructureBuilderBase {
public:
    OutputStructureBuilderRtoR();

    auto build() -> OutputManager * override;
};


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H
