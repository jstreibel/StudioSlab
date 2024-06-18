//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "hicpp-use-auto" // Don't warn use auto.
//#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast" // Don't warn use dynamic_cast
//

#include "RtoRNumericFunctionGPU.h"
#include "RtoRFunction.h"
#include "Math/Function/GPUFriendly.h"
#include "Math/Function/RtoR/Model/Operators/DerivativesGPU.h"

// #include <thrust/iterator/constant_iterator.h>

// Created by joao on 17/09/2019.

namespace Slab::Math::RtoR {

    NumericFunctionGPU::NumericFunctionGPU(UInt N, Real xMin, Real xMax, LaplacianType laplacianType)
            : NumericFunction(N, xMin, xMax, device::GPU, laplacianType) {

    }

    Base::NumericFunction <Real, Real> &NumericFunctionGPU::Set(const RtoR::Function &func) {
        auto &spaceGPU = dynamic_cast<DiscreteSpaceGPU &>(getSpace());

        auto &XHost = spaceGPU.getHostData(false);
        auto &XDev = spaceGPU.getDeviceData();

        if (!func.isGPUFriendly()) {
            const floatt L = xMax - xMin;
            for (UInt n = 0; n < N; n++) {
                const floatt x = L * n / (N - 1) + xMin;

                XHost[n] = func(x);
            }

            thrust::copy(std::begin(XHost), std::end(XHost), XDev.begin());
            spaceGPU.notifyHostIsUpdated();

            return *this;
        } else {
            SlabCast(inFunc, const RtoR::NumericFunction&, func);

            NOT_IMPLEMENTED
            // return RtoR::NumericFunction::Set(inFunc);
        }

        throw "Leftism has won, if you see this exception.";
    }


    Base::NumericFunction <Real, Real> &NumericFunctionGPU::Apply(const RtoR::Function &func,
                                                                  Base::NumericFunction <Real, Real> &out) const {
        assert(out.isGPUFriendly());

        const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU &>(getSpace());
        auto &outSpace = dynamic_cast<DiscreteSpaceGPU &>(out.getSpace());

        auto &XHost = mySpace.getHostData(false);
        auto &XDev = mySpace.getDeviceData();
        auto &outXDev = outSpace.getDeviceData();

        const Base::GPUFriendly &funcGPU = func.getGPUFriendlyVersion();
        //cast(outFunc, FunctionArbitraryGPU&, out);

        funcGPU.GPUApply(XDev, outXDev);

        return out;
    }

    NumericFunction &NumericFunctionGPU::Laplacian(NumericFunction &outFunc) const {
        if (laplacianType == NumericFunction::RadialSymmetry2D) throw "GPU RadialSymmetry2D laplacian not implemented.";

        // cast(out, FunctionArbitraryGPU&, outFunc);

        auto &outSpace = dynamic_cast<DiscreteSpaceGPU &>(outFunc.getSpace());

        const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU &>(getSpace());
        auto &XDev = mySpace.getDeviceData();
        auto &outXDev = outSpace.getDeviceData();

        auto h = getSpace().getMetaData().geth(0);
        auto is_periodic = laplacianType==Standard1D_PeriodicBorder;
        d2dx2(XDev, outXDev, h, N, is_periodic);

        return outFunc;
    }

    Str NumericFunctionGPU::myName() const {
        return "ℝ ↦ ℝ GPU discrete";
    }

    Pointer<Base::FunctionT <Real, Real>> NumericFunctionGPU::Clone() const {
        return New<NumericFunctionGPU>(N, xMin, xMax, laplacianType);
    }


}
//#pragma clang diagnostic pop
