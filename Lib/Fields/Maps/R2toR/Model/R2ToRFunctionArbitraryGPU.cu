
#include "Studios/Util/Workaround/ABIDef.h"

#include <thrust/iterator/constant_iterator.h>
#include "R2ToRFunctionArbitraryGPU.h"
#include "Studios/Math/Maps/RtoR/Model/Derivatives/DerivativesGPU.h"
#include "Studios/Math/Core/GPUFriendly.h"

using namespace R2toR;


FunctionArbitraryGPU::FunctionArbitraryGPU(PosInt N, Real sMin, Real h)
    : FunctionArbitrary(N, N, sMin, sMin, h, device::GPU), XHost(*new VecFloat(N*N)), XDev(*new DeviceVector(N * N))
{

}

R2toR::FunctionArbitrary &R2toR::FunctionArbitraryGPU::Laplacian(R2toR::FunctionArbitrary &outFunc) const {
    cast(out, FunctionArbitraryGPU&, outFunc);

    ::d2dx2(XDev, out.XDev, getSpace().geth(), N, M);

    return outFunc;
}

//
// Created by joao on 27/09/2019.
Base::Function<Real2D, Real> *R2toR::FunctionArbitraryGPU::Clone() const {
    throw "R2toRMap::FunctionArbitraryGPU::Clone() not implemented";
}

Base::FunctionArbitrary<Real2D, Real> *R2toR::FunctionArbitraryGPU::CloneWithSize(PosInt N) const {
    throw "R2toRMap::FunctionArbitraryGPU::Clone(unsigned int) not implemented.";
}

Real R2toR::FunctionArbitraryGPU::At(PosInt n, PosInt m) const {
    FunctionArbitraryGPU& me = const_cast<FunctionArbitraryGPU&>(*this);

    if (!hostIsUpdated){
        thrust::copy(XDev.begin(), XDev.end(), XHost.begin());
        me.hostIsUpdated = true;
    }

    //PrintDensityThere(n, m, 20*XHost[n+m*N]);
    return XHost[n+m*N];
}

Real &R2toR::FunctionArbitraryGPU::At(PosInt n, PosInt m) {
    if (!hostIsUpdated){
        thrust::copy(XDev.begin(), XDev.end(), XHost.begin());
        hostIsUpdated = true;
    }

    return XHost[n+m*N];
}

Base::FunctionArbitrary<Real2D, Real> &
R2toR::FunctionArbitraryGPU::Set(const Base::FunctionArbitrary<Real2D, Real>::MyBase &func) {
    const floatt Lx = xMax - xMin;
    const floatt Ly = yMax - yMin;
    for (PosInt n = 0; n < N; n++) {
        for (PosInt m = 0; m < M; m++) {
            const floatt x = Lx * n / (N - 1) + xMin;
            const floatt y = Ly * m / (M - 1) + yMin;

            XHost[n+m*N] = func({x,y});
        }
    }

    thrust::copy(XHost.begin(), XHost.end(), XDev.begin());

    hostIsUpdated = true;
    return *this;
}

Base::FunctionArbitrary<Real2D, Real> &
R2toR::FunctionArbitraryGPU::SetArb(const Base::FunctionArbitrary<Real2D, Real> &func) {
    cast(toFunc, const FunctionArbitraryGPU&, func)
    auto &in = toFunc.XDev;
    thrust::copy(in.begin(), in.end(), XDev.begin());

    hostIsUpdated = false;

    return *this;
}

Base::FunctionArbitrary<Real2D, Real> &R2toR::FunctionArbitraryGPU::Apply(const Base::Function<Real, Real> &func,
                                                                          Base::FunctionArbitrary<Real2D, Real> &out) const {
    if(!func.isGPUFriendly()) throw "Error trying to apply on GPU a function that which not GPU friendly.";

    const Base::GPUFriendly &funcGPU = func.getGPUFriendlyVersion();
    cast(outFunc, FunctionArbitraryGPU&, out);

    funcGPU.ApplyTo(XDev, outFunc.XDev);

    FunctionArbitraryGPU& me = const_cast<FunctionArbitraryGPU&>(*this); //  naughty you.....
    me.hostIsUpdated = false;

    return out;
}

Base::FunctionArbitrary<Real2D, Real> &
R2toR::FunctionArbitraryGPU::Add(const Base::FunctionArbitrary<Real2D, Real> &toi) {
    cast(func, const FunctionArbitraryGPU&, toi);
    auto &in = func.XDev;
    auto &out = XDev;

    thrust::transform(in.begin(), in.end(),
                      out.begin(), out.begin(),
                      thrust::plus<Real>());

    hostIsUpdated = false;

    return *this;
}

Base::FunctionArbitrary<Real2D, Real> &
R2toR::FunctionArbitraryGPU::StoreAddition(const Base::FunctionArbitrary<Real2D, Real> &toi1,
                                           const Base::FunctionArbitrary<Real2D, Real> &toi2) {
    cast(func1, const FunctionArbitraryGPU&, toi1);
    cast(func2, const FunctionArbitraryGPU&, toi2);
    auto &in1 = func1.XDev;
    auto &in2 = func2.XDev;
    auto &out = XDev;

    thrust::transform(in1.begin(), in1.end(),
                      in2.begin(),
                      out.begin(),
                      thrust::plus<Real>());

    hostIsUpdated = false;

    return *this;
}

Base::FunctionArbitrary<Real2D, Real> &
R2toR::FunctionArbitraryGPU::StoreSubtraction(const Base::FunctionArbitrary<Real2D, Real> &aoi1,
                                              const Base::FunctionArbitrary<Real2D, Real> &aoi2) {
    cast(func1, const FunctionArbitraryGPU&, aoi1);
    cast(func2, const FunctionArbitraryGPU&, aoi2);
    auto &in1 = func1.XDev;
    auto &in2 = func2.XDev;
    auto &out = XDev;

    thrust::transform(in1.begin(), in1.end(),
                      in2.begin(),
                      out.begin(),
                      thrust::minus<Real>());

    hostIsUpdated = false;

    return *this;
}

Base::FunctionArbitrary<Real2D, Real> &R2toR::FunctionArbitraryGPU::Multiply(floatt a) {
    auto &in = XDev;
    auto &out = XDev;

    thrust::transform(in.begin(), in.end(),
                      thrust::make_constant_iterator(a),
                      out.begin(),
                      thrust::multiplies<Real>());

    hostIsUpdated = false;

    return *this;
}