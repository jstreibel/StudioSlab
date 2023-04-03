//
// Created by joao on 10/8/21.
//

#ifndef FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H
#define FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H

#include <Phys/Numerics/Output/OutputStructureBuilderBase.h>
#include "Fields/Mappings/RtoR/View/Graphics/RtoROutputOpenGL.h"

class OutputStructureBuilderRtoR : public OutputStructureBuilderBase {
public:
    OutputStructureBuilderRtoR();

    auto build(String outputFileName) -> OutputManager * override;

    virtual auto buildOpenGLOutput() -> RtoR::OutputOpenGL*;


};


#endif //FIELDS_OUTPUTSTRUCTUREBUILDERRTOR_H
