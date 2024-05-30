//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_R2TOCDISCRETEFUNCTION_H
#define STUDIOSLAB_R2TOCDISCRETEFUNCTION_H

#include "Math/Function/Function.h"
#include "Math/VectorSpace/Impl/ComplexSpace.h"

namespace Slab::Math::R2toC {

    class DiscreteFunction : public Base::FunctionT<Real2D, Complex>  {
        ComplexArray data;

    public:
        const Real x0, y0;
        const Real Lx, Ly;
        const Resolution N, M;

        DiscreteFunction(Resolution N, Resolution M, Real x0, Real y0, Real Lx, Real Ly);
        ~DiscreteFunction() = default;

        Complex operator()(Real2D x) const override;
        Complex &At(Real2D x);
        Complex &At(int i, int j);

        auto getData() const -> const ComplexArray&;
        auto getData() -> ComplexArray&;
    };

    DefinePointer(DiscreteFunction)

} // R2toC

#endif //STUDIOSLAB_R2TOCDISCRETEFUNCTION_H
