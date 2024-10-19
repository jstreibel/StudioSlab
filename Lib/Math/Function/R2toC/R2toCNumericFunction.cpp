//
// Created by joao on 12/09/23.
//

#include "R2toCNumericFunction.h"
#include "Math/Data/Data.h"

namespace Slab::Math::R2toC {

    NumericFunction::NumericFunction(Resolution N, Resolution M, Real x0, Real y0, Real Lx, Real Ly)
    : Data(generalName())
    , x0(x0), y0(y0)
    , Lx(Lx), Ly(Ly)
    , N(N), M(M)
    , data(N*M) {

    }

    Complex NumericFunction::operator()(Real2D x) const {
        return const_cast<NumericFunction*>(this)->At(x);
    }

    inline Complex &NumericFunction::At(Real2D x) {
        fix i = (int)floor((x.x-x0)/Lx)*(N-1);
        fix j = (int)floor((x.y-y0)/Ly)*(M-1);

        return At(i,j);
    }

    inline Complex &NumericFunction::At(int i, int j) {
        static Complex zero;

        if(i<0 || i>N-1 || j<0 || j>M-1) return zero;

        return data[i + j*N];
    }

    auto NumericFunction::getData() const -> const ComplexArray & {
        return data;
    }

    auto NumericFunction::getData() -> ComplexArray & {
        return data;
    }

    DataType NumericFunction::get_data_type() const {
            DataType type = "Numeric:RegularGrid:ℝ²↦ℂ:CPU";

            return type;
    }


} // R2toC