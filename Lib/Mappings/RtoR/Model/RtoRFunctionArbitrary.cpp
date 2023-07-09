//

#include "RtoRFunctionArbitrary.h"


//
// Created by joao on 29/08/2019.

using namespace RtoR;

DiscreteFunction::DiscreteFunction(const DiscreteFunction &toCopy)
    : DiscreteFunction::DiscreteFunction(toCopy.N, toCopy.xMin, toCopy.xMax, device::CPU, laplacianType){
    throw "DiscreteFunction::DiscreteFunction(const FunctionArbitrary &toCopy) not implemented.";
}

DiscreteFunction::DiscreteFunction(PosInt N, Real xMin, Real xMax, device dev, LaplacianType laplacianType)
    : Base::DiscreteFunction<Real, Real>(DimensionMetaData({N}), (xMax - xMin) / Real(N), dev), N(N),
      xMin(xMin), xMax(xMax), laplacianType(laplacianType)
{

}

auto DiscreteFunction::mapIntToPos(PosInt i) const -> Real {
    const floatt L = xMax-xMin;
    const Real h_2 = .5*getSpace().geth();
    return (L * Real(i)/Real(N-1) + xMin) + h_2;
}

auto DiscreteFunction::mapPosToInt(Real x) const -> PosInt {
    const Real h = getSpace().geth();
    return PosInt((x-xMin)/h);
}

Real DiscreteFunction::operator()(Real x) const {
    const floatt L = xMax-xMin;

    // TODO: fazer essa conversao um pouco mais consciente.
    int n = int((N-1)*(x-xMin)/L);

    // TODO: fazer uma macro para colocar o que esta na linha logo abaixo, de forma que no modo Release isso
    //  nao seja incluido no codigo.

    if(n<0 || n>N-1) return .0;

    n = n<0?0:n>N-1?N-1:n;
    //assert(n>=0 && n<N);

    return getSpace().getHostData()[n];
}



