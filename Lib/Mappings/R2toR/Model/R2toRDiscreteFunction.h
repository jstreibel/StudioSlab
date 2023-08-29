//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARY_H
#define V_SHAPE_R2TORFUNCTIONARBITRARY_H

#include "Math/Function/DiscreteFunction.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "R2toRFunction.h"

namespace R2toR {
    class Domain {
    public:
        Real getLx() const { return xMax-xMin; }
        Real getLy() const { return yMax-yMin; }
        const Real xMin, xMax, yMin, yMax;
    };

    typedef Core::FunctionT<Real, Real> FuncBase;
    typedef Core::DiscreteFunction<Real2D, Real> DiscrBase;

    class DiscreteFunction : public Core::DiscreteFunction<Real2D,Real> {
    protected:
        const PosInt N, M;
        const Real xMin, xMax,
                   yMin, yMax,
                   h;

    public:
        typedef Core::DiscreteFunction <Real2D, Real> DiscreteFunctionBase;

        DiscreteFunction(PosInt N, PosInt M, Real xMin, Real yMin, Real h, device dev);
        DiscreteFunction(const NumericConfig &, device);

        auto operator()(Real2D x) const -> Real override;
        auto operator()(Real2D x)       -> Real &;

        auto diff(int n, Real2D x) const -> Real        override;
        auto diff(int n)           const -> MyBase::Ptr override;

        virtual DiscreteFunction &Laplacian(DiscreteFunction &outFunc) const = 0;
        virtual Real              At(PosInt n, PosInt m)               const = 0;
        virtual Real             &At(PosInt n, PosInt m)                     = 0;

        auto getN()      const -> PosInt;
        auto getM()      const -> PosInt;
        auto getDomain() const -> Domain;
        auto myName()    const -> Str override;
    };
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARY_H
