//
// Created by joao on 19/09/19.
//

#ifndef V_SHAPE_R2TORFUNCTIONARBITRARY_H
#define V_SHAPE_R2TORFUNCTIONARBITRARY_H

#include "Math/Function/NumericFunction.h"
#include "R2toRFunction.h"

namespace Slab::Math::R2toR {
    class Domain {
    public:
        DevFloat getLx() const { return xMax-xMin; }
        DevFloat getLy() const { return yMax-yMin; }
        const DevFloat xMin, xMax, yMin, yMax;

        bool operator ==(const Domain &other) const {
            return Common::AreEqual(xMin, other.xMin)
                && Common::AreEqual(xMax, other.xMax)
                && Common::AreEqual(yMin, other.yMin)
                && Common::AreEqual(yMax, other.yMax);
        }
    };

    typedef Base::FunctionT<DevFloat, DevFloat> FuncBase;
    typedef Base::NumericFunction<Real2D, DevFloat> DiscrBase;

    class FNumericFunction : public Base::NumericFunction<Real2D,DevFloat> {

    protected:
        const UInt N, M;
        const DevFloat xMin, xMax,
                   yMin, yMax,
                   hx, hy;

    public:
        typedef Base::NumericFunction <Real2D, DevFloat> NumericFunctionBase;
        using NumericAlgebra<NumericFunctionBase>::operator=;

        FNumericFunction(UInt N, UInt M, DevFloat xMin, DevFloat yMin, DevFloat hx, DevFloat hy, Device dev);
        // NumericFunction(const NumericConfig &, Device);

        auto operator()(Real2D x) const -> DevFloat override;
        auto operator()(Real2D x)       -> DevFloat &;

        auto diff(int n, int i, int j) const -> DevFloat;
        auto diff(int n, Real2D x) const -> DevFloat        override;
        auto diff(int n)           const -> Function_ptr override;

        auto domainContainsPoint(Real2D x) const -> bool override;

        virtual FNumericFunction &Laplacian(FNumericFunction &outFunc) const = 0;

        virtual const DevFloat&
        At(UInt n, UInt m) const = 0;
        virtual DevFloat&
        At(UInt n, UInt m) = 0;

        float get_data_size_MiB() override;

        auto getN()      const -> UInt;
        auto getM()      const -> UInt;
        auto getDomain() const -> Domain;
        auto generalName()    const -> Str override;

        DevFloat max() const override;

        DevFloat min() const override;
    };

    DefinePointers(FNumericFunction)
}


#endif //V_SHAPE_R2TORFUNCTIONARBITRARY_H
