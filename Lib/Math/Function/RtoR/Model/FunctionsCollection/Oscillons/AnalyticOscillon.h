//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_ANALYTICOSCILLON_H
#define V_SHAPE_ANALYTICOSCILLON_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class AnalyticOscillon : public RtoR::Function {
    public:
        enum Bit {
            phi, dPhiDt
        };
    public:
        // AnalyticOscillon(const AnalyticOscillon &);

        AnalyticOscillon(DevFloat t, DevFloat v, DevFloat u, DevFloat alpha, bool xMirrored, bool phiMirrored, Bit bit = phi);
        AnalyticOscillon(DevFloat t, DevFloat x0, DevFloat l, DevFloat v, DevFloat u, DevFloat alpha, bool xMirrored, bool phiMirrored, Bit bit = phi);

        void set_t(DevFloat t);

        DevFloat operator()(DevFloat x) const override;

        TPointer<FunctionT<DevFloat, DevFloat>> Clone() const override;

        AnalyticOscillon &swap();

        void setBit(Bit bit);

        Str Symbol() const override;

        Str generalName() const override;

    private:
        DevFloat t0, x0, v, u, s1, s2, l, alpha;
        Bit bit;
    };
}

#endif //V_SHAPE_ANALYTICOSCILLON_H
