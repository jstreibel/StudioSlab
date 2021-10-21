#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "hicpp-use-auto"
//

#include "RtoRFunctionArbitraryCPU.h"
#include "FieldsApps/RtoR/Model/Derivatives/DerivativesCPU.h"

// Created by joao on 17/09/2019.

using namespace RtoR;

//
FunctionArbitraryCPU::FunctionArbitraryCPU(const FunctionArbitraryCPU &toCopy)
    : FunctionArbitrary(toCopy.N, toCopy.xMin, toCopy.xMax, device::CPU, toCopy.laplacianType) {

    auto &toCopyX = toCopy.getSpace().getHostData(true);

    for(PosInt n=0; n<N; n++)
        getSpace().getX()[n] = toCopyX[n];
}

RtoR::FunctionArbitraryCPU::FunctionArbitraryCPU(PosInt N, Real xLeft, Real xRight, LaplacianType laplacianType)
    : FunctionArbitrary(N, xLeft, xRight, device::CPU, laplacianType){

}

FunctionArbitraryCPU &FunctionArbitraryCPU::Set(const Function & function) {
    assert(!function.isDiscrete());
    const floatt L = xMax - xMin;
    VecFloat &X = getSpace().getX();

    OMP_PARALLEL_FOR(n, N){
        const floatt x = L * n / (N - 1) + xMin;

        X[n] = function(x);
    }
    
    return *this;
}

Base::Function<Real, Real> *FunctionArbitraryCPU::Clone() const {
    throw "Dafuk";
    return new FunctionArbitraryCPU(*this);
}

Base::FunctionArbitrary<Real, Real> *FunctionArbitraryCPU::CloneWithSize(PosInt outN) const {
    FunctionArbitraryCPU &newFunc = *new FunctionArbitraryCPU(outN, xMin, xMax, laplacianType);

    const VecFloat &X = getSpace().getX();
    const double inc_d = N / double(outN);

    DiscreteSpace &newSpace = newFunc.getSpace();
    for(PosInt i=0; i<outN; i++)
        newSpace.getX()[i] = X[int(i*inc_d)];;

    return &newFunc;
}

Base::FunctionArbitrary<Real, Real> &FunctionArbitraryCPU::Apply(const Function &func,
                                                                 Base::FunctionArbitrary<Real, Real> &out) const {
    auto &outSpace = out.getSpace();
    auto &outX = outSpace.getX();
    const auto &myX = getSpace().getX();

    OMP_PARALLEL_FOR(n, N)
        outX[n] = func(myX[n]);

    return out;
}

FunctionArbitrary &FunctionArbitraryCPU::Laplacian(FunctionArbitrary &out) const {

    if(laplacianType == RadialSymmetry2D)
    {
        auto &outX = out.getSpace().getX();
        const auto &myX = getSpace().getX();
        const auto &f = this->getSpace().getX();
        const Real invh = 1./getSpace().geth();
        const Real invh_2 = 1./(2.*getSpace().geth());
        const Real invhsqr = invh*invh;

        OMP_GET_BEGIN_END(begin, end, outX.size())

        for (int i = begin; i < end; i++) {
            if(i==0||i==outX.size()-1) {
                outX[i] = .0;
                continue;
            }

            const Real r = mapIntToPos(i);
            const Real ddr = invh_2 * (-f[i-1] + f[i+1]);
            const Real d2dr2 = invhsqr * ((f[i-1] + f[i+1]) - 2.0*f[i]);

            outX[i] = d2dr2 + ddr/r;
            //outX[i] = d2dr2;
        }
    }
    else if(laplacianType == Standard1D)
    {
        DerivativeCPU deriv(*this);
        deriv.d2fdx2_v(out.getSpace().getX());
    }

    return out;
}


#pragma clang diagnostic pop