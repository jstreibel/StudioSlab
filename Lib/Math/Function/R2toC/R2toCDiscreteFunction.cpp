//
// Created by joao on 12/09/23.
//

#include "R2toCDiscreteFunction.h"

namespace Slab::Math::R2toC {

    DiscreteFunction::DiscreteFunction(Resolution N, Resolution M, Real x0, Real y0, Real Lx, Real Ly)
    : x0(x0), y0(y0)
    , Lx(Lx), Ly(Ly)
    , N(N), M(M)
    , data(N*M) {

    }

    Complex DiscreteFunction::operator()(Real2D x) const {
        return const_cast<DiscreteFunction*>(this)->At(x);
    }

    inline Complex &DiscreteFunction::At(Real2D x) {
        fix i = (int)floor((x.x-x0)/Lx)*(N-1);
        fix j = (int)floor((x.y-y0)/Ly)*(M-1);

        return At(i,j);
    }

    inline Complex &DiscreteFunction::At(int i, int j) {
        static Complex zero;

        if(i<0 || i>N-1 || j<0 || j>M-1) return zero;

        return data[i + j*N];
    }

    auto DiscreteFunction::getData() const -> const ComplexArray & {
        return data;
    }

    auto DiscreteFunction::getData() -> ComplexArray & {
        return data;
    }




} // R2toC