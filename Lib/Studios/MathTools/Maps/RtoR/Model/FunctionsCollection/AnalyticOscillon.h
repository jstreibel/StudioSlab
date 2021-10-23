//
// Created by joao on 09/09/2019.
//

#ifndef V_SHAPE_ANALYTICOSCILLON_H
#define V_SHAPE_ANALYTICOSCILLON_H

#include "Studios/MathTools/Maps/RtoR/Model/RtoRFunction.h"

namespace RtoR {
    class AnalyticOscillon : public Function {
    public:
        enum Bit {
            phi, dPhiDt
        };
    public:
        AnalyticOscillon(const AnalyticOscillon &);

        AnalyticOscillon(Real t, Real v, Real u, Real alpha, bool xMirrored, bool phiMirrored, Bit bit = phi);
        AnalyticOscillon(Real t, Real x0, Real l, Real v, Real u, Real alpha, bool xMirrored, bool phiMirrored, Bit bit = phi);

        Real operator()(Real x) const override;

        Function<Real, Real> *Clone() const override;

        AnalyticOscillon &swap();

    private:
        Real t0, x0, v, u, s1, s2, l;
        Bit bit;
    };
}

#endif //V_SHAPE_ANALYTICOSCILLON_H
