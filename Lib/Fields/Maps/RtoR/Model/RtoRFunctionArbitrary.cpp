//

#include "RtoRFunctionArbitrary.h"
#include "Studios/Math/Maps/RtoR/Model/Derivatives/DerivativesCPU.h"
#include <omp.h>

//
// Created by joao on 29/08/2019.

using namespace RtoR;

FunctionArbitrary::FunctionArbitrary(const FunctionArbitrary &toCopy)
    : FunctionArbitrary::FunctionArbitrary(toCopy.N, toCopy.xMin, toCopy.xMax, device::CPU, laplacianType){
    throw "FunctionArbitrary::FunctionArbitrary(const FunctionArbitrary &toCopy) not implemented.";
}

FunctionArbitrary::FunctionArbitrary(PosInt N, Real xMin, Real xMax, device dev, LaplacianType laplacianType)
    : Base::FunctionArbitrary<Real, Real>(DimensionMetaData({N}), (xMax-xMin)/Real(N), dev), N(N),
      xMin(xMin), xMax(xMax), laplacianType(laplacianType)
{

}

auto FunctionArbitrary::mapIntToPos(PosInt i) const -> Real {
    const floatt L = xMax-xMin;
    const Real h_2 = .5*getSpace().geth();
    return (L * Real(i)/Real(N-1) + xMin) + h_2;
}

auto FunctionArbitrary::mapPosToInt(Real x) const -> PosInt {
    const Real h = getSpace().geth();
    return PosInt((x-xMin)/h);
}

Real FunctionArbitrary::operator()(Real x) const {
    const floatt L = xMax-xMin;

    // TODO: fazer essa conversao um pouco mais consciente.
    int n = int((N-1)*(x-xMin)/L);

    // TODO: fazer uma macro para colocar o que esta na linha logo abaixo, de forma que no modo Release isso
    //  nao seja incluido no codigo.
    n = n<0?0:n>N-1?N-1:n;
    assert(n>=0 && n<N);

    return getSpace().getX()[n];
}

