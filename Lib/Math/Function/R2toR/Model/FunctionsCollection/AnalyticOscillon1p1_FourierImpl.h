//
// Created by joao on 10/5/24.
//

#ifndef STUDIOSLAB_ANALYTICOSCILLON1P1_FOURIERIMPL_H
#define STUDIOSLAB_ANALYTICOSCILLON1P1_FOURIERIMPL_H


#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "AnalyticOscillon_1plus1d.h"

namespace Slab::Math::R2toR {

    class AnalyticOscillon1p1_FourierImpl  : public R2toR::Function {
        Real (*f)(Int, Real, Real);

        Real x_max, x_min;
        Real gamma;

    public:
        using Bit = RtoR::AnalyticOscillon::Bit;
        using Parameters = R2toR::AnalyticOscillon_1plus1d::OscillonParameters;

        Parameters parameters;
        int n_max;

        explicit AnalyticOscillon1p1_FourierImpl(const Parameters &parameters={0,1,0,0,0}, Int n_max=7);

        Real operator()(Real2D x) const override;

        auto Clone() const -> Pointer<Type> override;

        void setParams(Parameters);

        void setBit(Bit bit);
    };

} // Slab::Math::R2toR

#endif //STUDIOSLAB_ANALYTICOSCILLON1P1_FOURIERIMPL_H
