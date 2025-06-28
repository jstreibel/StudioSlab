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

    NumericFunction::NumericFunction(UInt N, DevFloat xMin, DevFloat xMax, Device dev, LaplacianType laplacianType)
    : Base::NumericFunction<DevFloat, DevFloat>(DimensionMetaData({N}, {(xMax - xMin) / DevFloat(N-1)}),
                                        dev)
    , N(N)
    , xMin(xMin)
    , xMax(xMax)
    , laplacianType(laplacianType) {

    }

    auto NumericFunction::mapIntToPos(UInt i) const -> DevFloat {
        const floatt L = xMax - xMin;
        const DevFloat h_2 = .5 * getSpace().getMetaData().geth(0);
        return (L * DevFloat(i) / DevFloat(N - 1) + xMin) + h_2;
    }

    auto NumericFunction::mapPosToInt(DevFloat x) const -> UInt {
        const DevFloat h = getSpace().getMetaData().geth(0);
        return UInt((x - xMin) / h);
    }

    DevFloat NumericFunction::operator()(DevFloat x) const {
        const floatt L = xMax - xMin;
        const floatt dx = getSpace().getMetaData().geth(0);

        // TODO: fazer essa conversao um pouco mais consciente.
        // int n = int(floor((N - 1) * (x - xMin) / L));
        int n = int(floor((N - 1) * (x+.5*dx - xMin) / L));

        // TODO: fazer uma macro para colocar o que esta na linha logo abaixo, de forma que no modo Release isso
        //  nao seja incluido no codigo.

        if (n < 0 || n > N - 1) return .0;

        n = n < 0 ? 0 : n > N - 1 ? N - 1 : n;
        //assert(n>=0 && n<N);

        return getSpace().getHostData()[n];
    }


}