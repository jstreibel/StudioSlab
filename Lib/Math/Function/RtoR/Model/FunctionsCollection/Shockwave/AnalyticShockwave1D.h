//
// Created by joao on 22/04/2021.
//

#ifndef V_SHAPE_ANALYTICSHOCKWAVE1D_H
#define V_SHAPE_ANALYTICSHOCKWAVE1D_H


#include "Utils/NativeTypes.h"

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class AnalyticShockwave1D : public RtoR::Function {
        Vector<DevFloat> xk;
        Vector<DevFloat> ak;
        Vector<DevFloat> zk;

        DevFloat Wk(DevFloat z, int k) const;

        DevFloat t = .0;
    public:
        void setT(DevFloat t);

    public:
        AnalyticShockwave1D(DevFloat a0);

        DevFloat operator()(DevFloat x) const override;

        bool isDiscrete() const override;
    };
}


#endif //V_SHAPE_ANALYTICSHOCKWAVE1D_H
