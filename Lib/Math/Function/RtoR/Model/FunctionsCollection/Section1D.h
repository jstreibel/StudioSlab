//
// Created by joao on 17/08/23.
//

#ifndef STUDIOSLAB_SECTION1D_H
#define STUDIOSLAB_SECTION1D_H


#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/RtoR2/ParametricCurve.h"


namespace Slab::Math::RtoR {
    class Section1D : public RtoR::Function {
        R2toR::Function_constptr function;
        RtoR2::ParametricCurve_constptr curve;

    public:
        Section1D(R2toR::Function_constptr function=nullptr, RtoR2::ParametricCurve_constptr curve=nullptr);

        Real operator()(Real x) const override;

        Math::PointSet_ptr renderToPointSet(RenderingOptions options) override;
    };

    DefinePointers(Section1D)
}


#endif //STUDIOSLAB_SECTION1D_H
