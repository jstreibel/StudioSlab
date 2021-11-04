//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H

#include <Phys/DiffMath/Numerics/Output/OutputStructureBuilderBase.h>

class OutputStructureBuilderRtoR : public OutputStructureBuilderBase {
public:
    OutputStructureBuilderRtoR();

    auto build(String outputFileName) -> OutputManager * override;
};


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H
