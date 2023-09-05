#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "hicpp-use-auto"
//

#include "RtoRDiscreteFunctionCPU.h"
#include "Maps/RtoR/Model/Derivatives/DerivativesCPU.h"

#include "Utils/OMPUtils.h"

// Created by joao on 17/09/2019.

using namespace RtoR;

//
FunctionArbitraryCPU::FunctionArbitraryCPU(const FunctionArbitraryCPU &toCopy)
    : DiscreteFunction(toCopy.N, toCopy.xMin, toCopy.xMax, device::CPU, toCopy.laplacianType) {

    auto &toCopyX = toCopy.getSpace().getHostData(true);

    for(PosInt n=0; n<N; n++)
        getSpace().getHostData()[n] = toCopyX[n];
}

FunctionArbitraryCPU::FunctionArbitraryCPU(const DiscreteFunction& toCopy)
    : DiscreteFunction(toCopy.N, toCopy.xMin, toCopy.xMax, device::CPU, toCopy.getLaplacianType()) {

    auto &toCopyX = toCopy.getSpace().getHostData(true);

    for(PosInt n=0; n<N; n++)
        getSpace().getHostData()[n] = toCopyX[n];
}

RtoR::FunctionArbitraryCPU::FunctionArbitraryCPU(PosInt N, Real xLeft, Real xRight, LaplacianType laplacianType)
    : DiscreteFunction(N, xLeft, xRight, device::CPU, laplacianType){

}

FunctionArbitraryCPU::FunctionArbitraryCPU(RealArray_I data, Real xLeft, Real xRight,
                                           DiscreteFunction::LaplacianType laplacianType)
                                           : DiscreteFunction(N, xLeft, xRight, device::CPU, laplacianType) {
    getSpace().getHostData() = data;
}

auto FunctionArbitraryCPU::Set(const RealArray &vec) -> FunctionArbitraryCPU & {
    assert(vec.size() == getSpace().getHostData().size());

    for(int i=0; i<vec.size(); ++i)
        this->getSpace().getHostData()[i] = vec[i];

    return *this;
}

FunctionArbitraryCPU &FunctionArbitraryCPU::Set(const FunctionT & function) {
    const floatt L = xMax - xMin;
    RealArray &X = getSpace().getHostData();

    OMP_PARALLEL_FOR(n, N){
        const floatt x = L * n / (N - 1) + xMin;

        X[n] = function(x);
    }

    return *this;
}

Core::FunctionT<Real, Real> *FunctionArbitraryCPU::Clone() const {
    return new FunctionArbitraryCPU(*this);
}

Core::DiscreteFunction<Real, Real> *FunctionArbitraryCPU::CloneWithSize(PosInt outN) const {
    FunctionArbitraryCPU &newFunc = *new FunctionArbitraryCPU(outN, xMin, xMax, laplacianType);

    const RealArray &X = getSpace().getHostData();
    const Real inc_d = N / Real(outN);

    DiscreteSpace &newSpace = newFunc.getSpace();
    for(PosInt i=0; i<outN; i++)
        newSpace.getHostData()[i] = X[int(i * inc_d)];;

    return &newFunc;
}

Core::DiscreteFunction<Real, Real> &FunctionArbitraryCPU::Apply(const FunctionT &func,
                                                                Core::DiscreteFunction<Real, Real> &out) const {
    auto &outSpace = out.getSpace();

    auto &outX = outSpace.getHostData();
    const auto &myX = getSpace().getHostData();

    OMP_PARALLEL_FOR(n, N)
        outX[n] = func(myX[n]);

    return out;
}

DiscreteFunction &FunctionArbitraryCPU::Laplacian(DiscreteFunction &out) const {

    if(laplacianType == RadialSymmetry2D)
    {
        auto &outX = out.getSpace().getHostData();
        const auto &myX = getSpace().getHostData();
        const auto &f = this->getSpace().getHostData();
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
        deriv.d2fdx2_v(out.getSpace().getHostData());
    }

    return out;
}

Real FunctionArbitraryCPU::integrate() const {

    auto sum = .0;
    for(const auto &v : this->getSpace().getHostData())
        sum += v;

    auto dx = (this->xMax - this->xMin) / this->N;
    return sum*dx;
}



#pragma clang diagnostic pop