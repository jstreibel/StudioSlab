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
        explicit Section1D(const R2toR::Function *function= nullptr, const RtoR2::ParametricCurve *curve= nullptr);
        Section1D(R2toR::Function_constptr function, RtoR2::ParametricCurve_constptr curve);

        Real operator()(Real x) const override;

        Math::PointSet_ptr renderToPointSet(RenderingOptions options) override;
    };

    DefinePointer(Section1D)
}


#endif //STUDIOSLAB_SECTION1D_H
