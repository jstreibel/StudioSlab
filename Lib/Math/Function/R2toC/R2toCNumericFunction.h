//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_R2TOCNumericFunction_H
#define STUDIOSLAB_R2TOCNumericFunction_H

#include "Math/Function/Function.h"
#include "Math/VectorSpace/Impl/ComplexSpace.h"

namespace Slab::Math::R2toC {

    class NumericFunction : public Base::FunctionT<Real2D, Complex>  {
        ComplexArray data;

    public:
        const Real x0, y0;
        const Real Lx, Ly;
        const Resolution N, M;

        NumericFunction(Resolution N, Resolution M, Real x0, Real y0, Real Lx, Real Ly);
        ~NumericFunction() = default;

        Complex operator()(Real2D x) const override;
        Complex &At(Real2D x);
        Complex &At(int i, int j);

        auto getData() const -> const ComplexArray&;
        auto getData() -> ComplexArray&;
    };

    DefinePointers(NumericFunction)

} // R2toC

#endif //STUDIOSLAB_R2TOCNumericFunction_H
