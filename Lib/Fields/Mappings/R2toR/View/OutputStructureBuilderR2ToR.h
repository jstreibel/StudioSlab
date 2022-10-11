//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H

#include <Phys/Numerics/Output/OutputStructureBuilderBase.h>
#include <Phys/Numerics/Output/OutputManager.h>

class OutputStructureBuilderR2toR : public OutputStructureBuilderBase {
public:


    auto build(String outputFileName) -> OutputManager* override;


};


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERR2TOR_H
