//
// Created by joao on 27/09/2019.
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
#pragma ide diagnostic ignored "hicpp-use-auto"

#include <thrust/iterator/constant_iterator.h>

#include "RtoRSystemGordonGPU.h"
#include "RtoRFunctionArbitraryGPU.h"
#include "Mappings/RtoR/Model/Derivatives/DerivativesGPU.h"

using namespace RtoR;


SystemGordonGPU::SystemGordonGPU(PosInt N) : temp(N) { }

struct GPUHamiltonianStepper
{
    const floatt dt;

    GPUHamiltonianStepper(floatt dt) : dt(dt) {}

    __host__ __device__
    static inline Real dVdphi(const Real &phi){
        // TODO: usar implementacao de sign(x) da GPU.
        return phi < 0 ? -1 : (phi > 0 ? 1 : 0);
    }

    __host__ __device__
    floatt operator ()(const floatt &phi, const floatt &d2Phidx2) const {
        return dt*(-GPUHamiltonianStepper::dVdphi(phi) + d2Phidx2);
    }
};

FieldState &SystemGordonGPU::dtF(const FieldState &in, FieldState &out, Real t, Real dt) {
    cast(inPhi, const DiscreteFunctionGPU&, in.getPhi());
    cast(inDPhiDt, const DiscreteFunctionGPU&, in.getDPhiDt());
    cast(outPhi, DiscreteFunctionGPU&, out.getPhi());
    cast(outDPhiDt, DiscreteFunctionGPU&, out.getDPhiDt());

    const auto& sInPhi = dynamic_cast<const DiscreteSpaceGPU&>(inPhi.getSpace());
    const auto& sInDPhiDt = dynamic_cast<const DiscreteSpaceGPU&>(inDPhiDt.getSpace());
    auto& sOutPhi = dynamic_cast<DiscreteSpaceGPU&>(outPhi.getSpace());
    auto& sOutDPhiDt = dynamic_cast<DiscreteSpaceGPU&>(outDPhiDt.getSpace());

    const DeviceVector& vInPhi = sInPhi.getDeviceData();
    const DeviceVector& vInDPhiDt = sInDPhiDt.getDeviceData();
    DeviceVector & vOutPhi = sOutPhi.getDeviceData();
    DeviceVector & vOutDPhiDt = sOutDPhiDt.getDeviceData();

    {
        thrust::transform(vInDPhiDt.begin(), vInDPhiDt.end(),
                          thrust::constant_iterator<Real>(dt), vOutPhi.begin(),
                          thrust::multiplies<Real>());
    }

    {
        const Real h = sInPhi.geth();
        const PosInt N = sInPhi.getTotalDiscreteSites();
        DeviceVector & d2dx2_v = temp;
        d2dx2(vInPhi, d2dx2_v, h, N);
        thrust::transform(vInPhi.begin(), vInPhi.end(),
                          d2dx2_v.begin(), vOutDPhiDt.begin(),
                          GPUHamiltonianStepper(dt));
    }

    return out;
}

#pragma clang diagnostic pop