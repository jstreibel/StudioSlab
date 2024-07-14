//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARY_H
#define V_SHAPE_R2TORFUNCTIONARBITRARY_H

#include "Math/Function/NumericFunction.h"
#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "R2toRFunction.h"

namespace Slab::Math::R2toR {
    class Domain {
    public:
        Real getLx() const { return xMax-xMin; }
        Real getLy() const { return yMax-yMin; }
        const Real xMin, xMax, yMin, yMax;

        bool operator ==(const Domain &other) const {
            return Common::AreEqual(xMin, other.xMin)
                && Common::AreEqual(xMax, other.xMax)
                && Common::AreEqual(yMin, other.yMin)
                && Common::AreEqual(yMax, other.yMax);
        }
    };

    typedef Base::FunctionT<Real, Real> FuncBase;
    typedef Base::NumericFunction<Real2D, Real> DiscrBase;

    class NumericFunction : public Base::NumericFunction<Real2D,Real> {
    protected:
        const UInt N, M;
        const Real xMin, xMax,
                   yMin, yMax,
                   hx, hy;

    public:
        typedef Base::NumericFunction <Real2D, Real> NumericFunctionBase;
        using NumericAlgebra<NumericFunctionBase>::operator=;

        NumericFunction(UInt N, UInt M, Real xMin, Real yMin, Real hx, Real hy, Device dev);
        // NumericFunction(const NumericConfig &, Device);

        auto operator()(Real2D x) const -> Real override;
        auto operator()(Real2D x)       -> Real &;

        auto diff(int n, int i, int j) const -> Real;
        auto diff(int n, Real2D x) const -> Real        override;
        auto diff(int n)           const -> Function_ptr override;

        auto domainContainsPoint(Real2D x) const -> bool override;

        virtual NumericFunction &Laplacian(NumericFunction &outFunc) const = 0;

        virtual const Real&
        At(UInt n, UInt m) const = 0;
        virtual Real&
        At(UInt n, UInt m) = 0;

        auto getN()      const -> UInt;
        auto getM()      const -> UInt;
        auto getDomain() const -> Domain;
        auto myName()    const -> Str override;

        Real max() const override;

        Real min() const override;
    };

    DefinePointers(NumericFunction)
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARY_H
