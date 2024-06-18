//

#include "RtoRNumericFunction.h"

//
// Created by joao on 29/08/2019.

namespace Slab::Math::RtoR {

    NumericFunction::NumericFunction(const NumericFunction &toCopy)
            : NumericFunction::NumericFunction(toCopy.N,
                                                 toCopy.xMin,
                                                 toCopy.xMax,
                                                 toCopy.dev,
                                                 toCopy.laplacianType) {}

    NumericFunction::NumericFunction(UInt N, Real xMin, Real xMax, Device dev, LaplacianType laplacianType)
    : Base::NumericFunction<Real, Real>(DimensionMetaData({N}, {(xMax - xMin) / Real(N)}),
                                        dev)
    , N(N)
    , xMin(xMin)
    , xMax(xMax)
    , laplacianType(laplacianType) {

    }

    auto NumericFunction::mapIntToPos(UInt i) const -> Real {
        const floatt L = xMax - xMin;
        const Real h_2 = .5 * getSpace().getMetaData().geth(0);
        return (L * Real(i) / Real(N - 1) + xMin) + h_2;
    }

    auto NumericFunction::mapPosToInt(Real x) const -> UInt {
        const Real h = getSpace().getMetaData().geth(0);
        return UInt((x - xMin) / h);
    }

    Real NumericFunction::operator()(Real x) const {
        const floatt L = xMax - xMin;

        // TODO: fazer essa conversao um pouco mais consciente.
        int n = int(floor((N - 1) * (x - xMin) / L));

        // TODO: fazer uma macro para colocar o que esta na linha logo abaixo, de forma que no modo Release isso
        //  nao seja incluido no codigo.

        if (n < 0 || n > N - 1) return .0;

        n = n < 0 ? 0 : n > N - 1 ? N - 1 : n;
        //assert(n>=0 && n<N);

        return getSpace().getHostData()[n];
    }


}