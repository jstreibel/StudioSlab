//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "hicpp-use-auto" // Don't warn use auto.
//#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast" // Don't warn use dynamic_cast
//

#include "RtoRFunctionArbitraryGPU.h"

#include "Mappings/RtoR/Model/Derivatives/DerivativesGPU.h"
#include "Phys/Function/GPUFriendly.h"

// #include <thrust/iterator/constant_iterator.h>

// Created by joao on 17/09/2019.

using namespace RtoR;

FunctionArbitraryGPU::FunctionArbitraryGPU(PosInt N, Real xMin, Real xMax, LaplacianType laplacianType)
    : ArbitraryFunction(N, xMin, xMax, device::GPU, laplacianType) {

}

Base::ArbitraryFunction<Real, Real> &FunctionArbitraryGPU::Set(const RtoR::Function &func) {
    DiscreteSpaceGPU &spaceGPU = dynamic_cast<DiscreteSpaceGPU&>(getSpace());

    auto& XHost = spaceGPU.getHostData(false);
    auto& XDev = spaceGPU.getXDev();

    if(!func.isGPUFriendly()) {
        const floatt L = xMax - xMin;
        for (PosInt n = 0; n < N; n++) {
            const floatt x = L * n / (N - 1) + xMin;

            XHost[n] = func(x);
        }

        thrust::copy(XHost.begin(), XHost.end(), XDev.begin());
        spaceGPU.notifyHostIsUpdated();

        return *this;
    } else {
        cast(inFunc, const RtoR::ArbitraryFunction&, func);
        return Set(inFunc);
    }

    throw "Liberals have won, if you see this exception.";
}



Base::ArbitraryFunction<Real,Real> &FunctionArbitraryGPU::Apply(const RtoR::Function &func,
                                                                Base::ArbitraryFunction<Real,Real> &out) const {
    assert(out.isGPUFriendly());

    const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU&>(getSpace());
    auto &outSpace = dynamic_cast<DiscreteSpaceGPU&>(out.getSpace());

    auto& XHost = mySpace.getHostData(false);
    auto& XDev = mySpace.getXDev();
    auto& outXDev = outSpace.getXDev();

    const Base::GPUFriendly &funcGPU = func.getGPUFriendlyVersion();
    //cast(outFunc, FunctionArbitraryGPU&, out);

    funcGPU.GPUApply(XDev, outXDev);

    return out;
}

ArbitraryFunction &FunctionArbitraryGPU::Laplacian(ArbitraryFunction &outFunc) const {
    if(laplacianType == ArbitraryFunction::LaplacianType::RadialSymmetry2D) throw "GPU RadialSymmetry2D laplacian not implemented.";

    // cast(out, FunctionArbitraryGPU&, outFunc);

    auto &outSpace = dynamic_cast<DiscreteSpaceGPU&>(outFunc.getSpace());

    const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU&>(getSpace());
    auto& XDev = mySpace.getXDev();
    auto& outXDev = outSpace.getXDev();

    ::d2dx2(XDev, outXDev, getSpace().geth(), N);

    return outFunc;
}

//#pragma clang diagnostic pop