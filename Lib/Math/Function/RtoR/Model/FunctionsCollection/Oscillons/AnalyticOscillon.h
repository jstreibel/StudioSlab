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
        AnalyticOscillon(const AnalyticOscillon &);

        AnalyticOscillon(Real t, Real v, Real u, Real alpha, bool xMirrored, bool phiMirrored, Bit bit = phi);
        AnalyticOscillon(Real t, Real x0, Real l, Real v, Real u, Real alpha, bool xMirrored, bool phiMirrored, Bit bit = phi);

        void set_t(Real t);

        Real operator()(Real x) const override;

        Pointer<FunctionT<Real, Real>> Clone() const override;

        AnalyticOscillon &swap();

        void setBit(Bit bit);

        Str symbol() const override;

        Str myName() const override;

    private:
        Real t0, x0, v, u, s1, s2, l, alpha;
        Bit bit;
    };
}

#endif //V_SHAPE_ANALYTICOSCILLON_H
