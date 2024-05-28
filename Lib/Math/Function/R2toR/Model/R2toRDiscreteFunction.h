//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARY_H
#define V_SHAPE_R2TORFUNCTIONARBITRARY_H

#include "Math/Function/DiscreteFunction.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "R2toRFunction.h"

namespace Slab::Math::R2toR {
    class Domain {
    public:
        Real getLx() const { return xMax-xMin; }
        Real getLy() const { return yMax-yMin; }
        const Real xMin, xMax, yMin, yMax;
    };

    typedef Base::FunctionT<Real, Real> FuncBase;
    typedef Base::DiscreteFunction<Real2D, Real> DiscrBase;

    class DiscreteFunction : public Base::DiscreteFunction<Real2D,Real> {
    protected:
        const UInt N, M;
        const Real xMin, xMax,
                   yMin, yMax,
                   hx, hy;

    public:
        typedef Base::DiscreteFunction <Real2D, Real> DiscreteFunctionBase;

        DiscreteFunction(UInt N, UInt M, Real xMin, Real yMin, Real hx, Real hy, device dev);
        DiscreteFunction(const NumericConfig &, device);

        auto operator()(Real2D x) const -> Real override;
        auto operator()(Real2D x)       -> Real &;

        auto diff(int n, Real2D x) const -> Real        override;
        auto diff(int n)           const -> Function_ptr override;

        auto domainContainsPoint(Real2D x) const -> bool override;

        virtual DiscreteFunction &Laplacian(DiscreteFunction &outFunc) const = 0;
        virtual Real              At(UInt n, UInt m)               const = 0;
        virtual Real             &At(UInt n, UInt m)                     = 0;

        auto getN()      const -> UInt;
        auto getM()      const -> UInt;
        auto getDomain() const -> Domain;
        auto myName()    const -> Str override;

        Real max() const override;

        Real min() const override;
    };

    DefinePointer(DiscreteFunction)
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARY_H
