//
// Created by joao on 10/19/22.
//

#ifndef STUDIOSLAB_OUTPUTBUILDER_H
#define STUDIOSLAB_OUTPUTBUILDER_H

#include <Fields/Mappings/R2toR/View/OutputStructureBuilderR2ToR.h>

#include "GrowingHole/GrowingHoleGLOutput.h"
#include "Phys/Numerics/Allocator.h"


class OutputBuilder : public OutputStructureBuilderR2toR {
protected:
    auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override {
        const double phiMin = -.5;
        const double phiMax = 1;

        const Real xLeft = Allocator::getInstance().getNumericParams().getxLeft();
        const Real xRight = xLeft + Allocator::getInstance().getNumericParams().getL();

        return new R2toR::GrowingHoleOutGL(xLeft, xRight, xLeft, xRight, phiMin, phiMax);
    }
};




#endif //STUDIOSLAB_OUTPUTBUILDER_H
