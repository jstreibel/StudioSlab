//
// Created by joao on 12/09/23.
//

#include "R2toCDiscreteFunction.h"

namespace R2toC {

    DiscreteFunction::DiscreteFunction(Resolution N, Resolution M, Real x0, Real y0, Real Lx, Real Ly)
    : space(DimensionMetaData({(UInt)N,(UInt)M}, {Lx/N, Ly/M}))
    , x0(x0), y0(y0)
    , Lx(Lx), Ly(Ly)
    , N(N), M(M) {

    }

    Complex DiscreteFunction::operator()(Real2D x) const {
        return const_cast<DiscreteFunction*>(this)->operator()(x);
    }

    inline Complex &DiscreteFunction::At(Real2D x) {
        fix i = (int)floor((x.x-x0)/Lx)*(N-1);
        fix j = (int)floor((x.y-y0)/Ly)*(M-1);

        return At(i,j);
    }

    inline Complex &DiscreteFunction::At(int i, int j) {
        static Complex zero;

        if(i<0 || i>N-1 || j<0 || j>M-1) return zero;

        return space.getData()[i + j*N];
    }


} // R2toC