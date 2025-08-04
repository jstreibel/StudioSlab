//
// Created by joao on 22/04/2021.
//

#ifndef V_SHAPE_ANALYTICSHOCKWAVE1D_H
#define V_SHAPE_ANALYTICSHOCKWAVE1D_H


#include "Utils/NativeTypes.h"

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class AnalyticShockwave1D : public RtoR::Function {
        Vector<Real> xk;
        Vector<Real> ak;
        Vector<Real> zk;

        Real Wk(Real z, int k) const;

        Real t = .0;
    public:
        void setT(Real t);

    public:
        AnalyticShockwave1D(Real a0);

        Real operator()(Real x) const override;

        bool isDiscrete() const override;
    };
}


#endif //V_SHAPE_ANALYTICSHOCKWAVE1D_H
