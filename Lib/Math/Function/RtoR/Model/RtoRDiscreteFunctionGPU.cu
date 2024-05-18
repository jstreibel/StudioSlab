//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "hicpp-use-auto" // Don't warn use auto.
//#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast" // Don't warn use dynamic_cast
//

#include "RtoRDiscreteFunctionGPU.h"

#include "Math/Function/RtoR/Model/Derivatives/DerivativesGPU.h"
#include "Math/Function/GPUFriendly.h"

// #include <thrust/iterator/constant_iterator.h>

// Created by joao on 17/09/2019.

using namespace RtoR;

DiscreteFunctionGPU::DiscreteFunctionGPU(UInt N, Real xMin, Real xMax, LaplacianType laplacianType)
    : DiscreteFunction(N, xMin, xMax, device::GPU, laplacianType) {

}

Core::DiscreteFunction<Real, Real> &DiscreteFunctionGPU::Set(const RtoR::Function &func) {
    DiscreteSpaceGPU &spaceGPU = dynamic_cast<DiscreteSpaceGPU&>(getSpace());

    auto& XHost = spaceGPU.getHostData(false);
    auto& XDev = spaceGPU.getDeviceData();

    if(!func.isGPUFriendly()) {
        const floatt L = xMax - xMin;
        for (UInt n = 0; n < N; n++) {
            const floatt x = L * n / (N - 1) + xMin;

            XHost[n] = func(x);
        }

        thrust::copy(std::begin(XHost), std::end(XHost), XDev.begin());
        spaceGPU.notifyHostIsUpdated();

        return *this;
    } else {
        cast(inFunc, const RtoR::DiscreteFunction&, func);
        return Set(inFunc);
    }

    throw "Leftism has won, if you see this exception.";
}



Core::DiscreteFunction<Real,Real> &DiscreteFunctionGPU::Apply(const RtoR::Function &func,
                                                              Core::DiscreteFunction<Real,Real> &out) const {
    assert(out.isGPUFriendly());

    const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU&>(getSpace());
    auto &outSpace = dynamic_cast<DiscreteSpaceGPU&>(out.getSpace());

    auto& XHost = mySpace.getHostData(false);
    auto& XDev = mySpace.getDeviceData();
    auto& outXDev = outSpace.getDeviceData();

    const Core::GPUFriendly &funcGPU = func.getGPUFriendlyVersion();
    //cast(outFunc, FunctionArbitraryGPU&, out);

    funcGPU.GPUApply(XDev, outXDev);

    return out;
}

DiscreteFunction &DiscreteFunctionGPU::Laplacian(DiscreteFunction &outFunc) const {
    if(laplacianType == DiscreteFunction::RadialSymmetry2D) throw "GPU RadialSymmetry2D laplacian not implemented.";

    // cast(out, FunctionArbitraryGPU&, outFunc);

    auto &outSpace = dynamic_cast<DiscreteSpaceGPU&>(outFunc.getSpace());

    const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU&>(getSpace());
    auto& XDev = mySpace.getDeviceData();
    auto& outXDev = outSpace.getDeviceData();

    ::d2dx2(XDev, outXDev, getSpace().geth(), N);

    return outFunc;
}

Str DiscreteFunctionGPU::myName() const {
    return "ℝ ↦ ℝ GPU discrete";
}

Core::FunctionT<Real, Real> *DiscreteFunctionGPU::Clone() const {
    return new DiscreteFunctionGPU(N, xMin, xMax, laplacianType);
}

//#pragma clang diagnostic pop
