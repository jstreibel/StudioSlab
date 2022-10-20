

#include <thrust/iterator/constant_iterator.h>
#include "R2ToRFunctionArbitraryGPU.h"
#include "Fields/Mappings/R2toR/Model/Derivatives/DerivativesGPU.h"


using namespace R2toR;

#include <Phys/Function/GPUFriendly.h>


FunctionArbitraryGPU::FunctionArbitraryGPU(PosInt N, Real sMin, Real h)
    : FunctionArbitrary(N, N, sMin, sMin, h, device::GPU), XHost(*new VecFloat(N*N)),
                        XDev(*new DeviceVector(N * N))
{

}

R2toR::FunctionArbitrary &R2toR::FunctionArbitraryGPU::Laplacian(R2toR::FunctionArbitrary &outFunc) const {
    cast(out, FunctionArbitraryGPU&, outFunc);

    d2dx2(XDev, out.XDev, getSpace().geth(), N, M);

    return outFunc;
}

//
// Created by joao on 27/09/2019.
Base::Function<Real2D, Real> *R2toR::FunctionArbitraryGPU::Clone() const {
    throw "R2toRMap::FunctionArbitraryGPU::Clone() not implemented";
}

Base::ArbitraryFunction<Real2D, Real> *R2toR::FunctionArbitraryGPU::CloneWithSize(PosInt N) const {
    throw "R2toRMap::FunctionArbitraryGPU::Clone(unsigned int) not implemented.";
}

Real R2toR::FunctionArbitraryGPU::At(PosInt n, PosInt m) const {
    const_cast<FunctionArbitraryGPU&>(*this).updateHost();

    //PrintDensityThere(n, m, 20*XHost[n+m*N]);
    return XHost[n+m*N];
}

Real &R2toR::FunctionArbitraryGPU::At(PosInt n, PosInt m) {
    updateHost();

    return XHost[n+m*N];
}

Base::ArbitraryFunction<Real2D, Real> &
R2toR::FunctionArbitraryGPU::Set(const Base::ArbitraryFunction<Real2D, Real>::MyBase &func) {
    //updateHost();

    const floatt Lx = xMax - xMin;
    const floatt Ly = yMax - yMin;
    for (PosInt n = 0; n < N; n++) {
        for (PosInt m = 0; m < M; m++) {
            const floatt x = Lx * n / (N - 1) + xMin;
            const floatt y = Ly * m / (M - 1) + yMin;

            const Real2D r = {x, y};

            if(!func.domainContainsPoint(r))
                continue;

            XHost[n+m*N] = func(r);
        }
    }

    updateDevice();

    return *this;
}

Base::ArbitraryFunction<Real2D, Real> &
R2toR::FunctionArbitraryGPU::SetArb(const Base::ArbitraryFunction<Real2D, Real> &func) {
    cast(toFunc, const FunctionArbitraryGPU&, func)
    auto &in = toFunc.XDev;
    thrust::copy(in.begin(), in.end(), XDev.begin());

    hostIsSyncd = false;

    return *this;
}

Base::ArbitraryFunction<Real2D, Real> &
        R2toR::FunctionArbitraryGPU::Apply(const Base::Function<Real, Real> &func,
                                           Base::ArbitraryFunction<Real2D, Real> &out) const {
    assert(func.isGPUFriendly());

    const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU&>(getSpace());
    auto &outSpace = dynamic_cast<DiscreteSpaceGPU&>(out.getSpace());

    auto& XHost = mySpace.getHostData(false);
    auto& inX = mySpace.getXDev();
    auto& outX = outSpace.getXDev();

    const Base::GPUFriendly &funcGPU = func.getGPUFriendlyVersion();
    //cast(outFunc, FunctionArbitraryGPU&, out);

    funcGPU.GPUApply(inX, outX);

    return out;
}

Base::ArbitraryFunction<Real2D, Real> &
R2toR::FunctionArbitraryGPU::Add(const Base::ArbitraryFunction<Real2D, Real> &toi) {
    cast(func, const FunctionArbitraryGPU&, toi);
    auto &in = func.XDev;
    auto &out = XDev;

    thrust::transform(in.begin(), in.end(),
                      out.begin(), out.begin(),
                      thrust::plus<Real>());

    hostIsSyncd = false;

    return *this;
}

Base::ArbitraryFunction<Real2D, Real> &
R2toR::FunctionArbitraryGPU::StoreAddition(const Base::ArbitraryFunction<Real2D, Real> &toi1,
                                           const Base::ArbitraryFunction<Real2D, Real> &toi2) {
    cast(func1, const FunctionArbitraryGPU&, toi1);
    cast(func2, const FunctionArbitraryGPU&, toi2);
    auto &in1 = func1.XDev;
    auto &in2 = func2.XDev;
    auto &out = XDev;

    thrust::transform(in1.begin(), in1.end(),
                      in2.begin(),
                      out.begin(),
                      thrust::plus<Real>());

    hostIsSyncd = false;

    return *this;
}

Base::ArbitraryFunction<Real2D, Real> &
R2toR::FunctionArbitraryGPU::StoreSubtraction(const Base::ArbitraryFunction<Real2D, Real> &aoi1,
                                              const Base::ArbitraryFunction<Real2D, Real> &aoi2) {
    cast(func1, const FunctionArbitraryGPU&, aoi1);
    cast(func2, const FunctionArbitraryGPU&, aoi2);
    auto &in1 = func1.XDev;
    auto &in2 = func2.XDev;
    auto &out = XDev;

    thrust::transform(in1.begin(), in1.end(),
                      in2.begin(),
                      out.begin(),
                      thrust::minus<Real>());

    hostIsSyncd = false;

    return *this;
}

Base::ArbitraryFunction<Real2D, Real> &R2toR::FunctionArbitraryGPU::Multiply(floatt a) {
    auto &in = XDev;
    auto &out = XDev;

    thrust::transform(in.begin(), in.end(),
                      thrust::make_constant_iterator(a),
                      out.begin(),
                      thrust::multiplies<Real>());

    hostIsSyncd = false;

    return *this;
}

void FunctionArbitraryGPU::updateHost() {
    if (!hostIsSyncd){
        thrust::copy(XDev.begin(), XDev.end(), XHost.begin());
        hostIsSyncd = true;
    }
}

void FunctionArbitraryGPU::updateDevice() {
    if(!hostIsSyncd) {
        thrust::copy(XHost.begin(), XHost.end(), XDev.begin());
        hostIsSyncd = true;
    }
}

inline bool FunctionArbitraryGPU::isSyncd() const {
    return hostIsSyncd;
}
