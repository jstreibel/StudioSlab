//
// Created by joao on 10/19/22.
//

#ifndef STUDIOSLAB_OUTPUTBUILDER_H
#define STUDIOSLAB_OUTPUTBUILDER_H

#include "Mappings/R2toR/View/OutputStructureBuilderR2ToR.h"

#include "GrowingHoleGLOutput.h"
#include "Phys/Numerics/Allocator.h"

namespace R2toR {
    namespace GrowingHole {

        class OutputBuilder : public R2toR::OutputSystem::Builder {
        protected:
            auto buildOpenGLOutput() -> R2toR::OutputOpenGL * override {
                const Real phiMin = -.5;
                const Real phiMax = 1;

                auto &p = Numerics::Allocator::GetInstance().getNumericParams();

                const Real xLeft = p.getxLeft();
                const Real xRight = xLeft + p.getL();

                return new R2toR::GrowingHoleOutGL(p, xLeft, xRight, xLeft, xRight, phiMin, phiMax);
            }

        public:
            OutputBuilder() : Builder("GH output builder", "Growing hole output system builder") {}
        };
    }
}





#endif //STUDIOSLAB_OUTPUTBUILDER_H
