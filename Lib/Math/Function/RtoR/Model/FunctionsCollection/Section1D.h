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
        Pointer<const R2toR::Function> function;
        Pointer<const RtoR2::ParametricCurve> curve;

    public:
        Section1D(Pointer<const R2toR::Function> function=nullptr, Pointer<const RtoR2::ParametricCurve> curve=nullptr);

        DevFloat operator()(DevFloat x) const override;

        Pointer<Math::PointSet> renderToPointSet(RenderingOptions options) override;
    };

    DefinePointers(Section1D)
}


#endif //STUDIOSLAB_SECTION1D_H
