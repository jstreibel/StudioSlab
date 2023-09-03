//
// Created by joao on 17/08/23.
//

#ifndef STUDIOSLAB_SECTION1D_H
#define STUDIOSLAB_SECTION1D_H


#include "Mappings/RtoR/Model/RtoRFunction.h"
#include "Mappings/R2toR/Model/R2toRFunction.h"
#include "Mappings/RtoR2/ParametricCurve.h"


namespace RtoR {
    class Section1D : public RtoR::Function {
        R2toR::Function::ConstPtr function;
        RtoR2::ParametricCurve::ConstPtr curve;

    public:
        explicit Section1D(const R2toR::Function *function= nullptr, const RtoR2::ParametricCurve *curve= nullptr);
        Section1D(R2toR::Function::ConstPtr function, RtoR2::ParametricCurve::ConstPtr curve);

        Real operator()(Real x) const override;

        Spaces::PointSet::Ptr renderToPointSet(RenderingOptions options) override;
    };
}


#endif //STUDIOSLAB_SECTION1D_H
