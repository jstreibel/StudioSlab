


#include "R2toRDiscreteFunctionGPU.h"

#include "Mappings/R2toR/Model/Derivatives/DerivativesGPU.h"
#include "Phys/Function/GPUFriendly.h"



using namespace R2toR;



FunctionArbitraryGPU::FunctionArbitraryGPU(PosInt N, Real sMin, Real h)
    : DiscreteFunction(N, N, sMin, sMin, h, device::GPU)//, XHost(*new VecFloat(N*N)), XDev(*new DeviceVector(N * N))
{

}

FunctionArbitraryGPU::~FunctionArbitraryGPU() {
    if(helper != nullptr) delete helper;
}

R2toR::DiscreteFunction &R2toR::FunctionArbitraryGPU::Laplacian(R2toR::DiscreteFunction &outFunc) const {
    // assert(getSpace().isCompatible(outFunc.getSpace()))

    cast(inSpace, const DiscreteSpaceGPU&, getSpace())
    cast(outSpace, DiscreteSpaceGPU&, outFunc.getSpace())

    d2dx2(inSpace.getDeviceData(), outSpace.getDeviceData(), inSpace.geth(), N, M);

    return outFunc;
}

//
// Created by joao on 27/09/2019.
Base::Function<Real2D, Real> *R2toR::FunctionArbitraryGPU::Clone() const {
    throw "R2toRMap::FunctionArbitraryGPU::Clone() not implemented";
}

Base::DiscreteFunction<Real2D, Real> *R2toR::FunctionArbitraryGPU::CloneWithSize(PosInt N) const {
    throw "R2toRMap::FunctionArbitraryGPU::Clone(unsigned int) not implemented.";
}

Real R2toR::FunctionArbitraryGPU::At(PosInt n, PosInt m) const {
    return getSpace().getHostData()[n + m * N];

    // const_cast<FunctionArbitraryGPU&>(*this).updateHost();

    // PrintDensityThere(n, m, 20*XHost[n+m*N]);

    // return XHost[n+m*N];
}

Real &R2toR::FunctionArbitraryGPU::At(PosInt n, PosInt m) {
    return getSpace().getHostData()[n + m * N];

    //updateHost();

    //return XHost[n+m*N];
}

DiscreteFunction &
R2toR::FunctionArbitraryGPU::Set(const MyBase &func) {
    if(func.isDiscrete()) {
        auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(func);
    }

    auto &space = getSpace();

    space.syncHost();

    auto &XHost = space.getHostData(false);

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

    space.upload();

    return *this;
}

DiscreteFunction &
R2toR::FunctionArbitraryGPU::SetArb(const Base::DiscreteFunction<Real2D, Real> &func) {
    getSpace().setToValue(func.getSpace());

    return *this;
}

Base::DiscreteFunction<Real2D, Real> &FunctionArbitraryGPU::Apply(const Function<Real, Real> &func,
                                                                  Base::DiscreteFunction<Real2D, Real> &out) const {
    assert(func.isGPUFriendly());

    const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU&>(getSpace());
    auto &outSpace = dynamic_cast<DiscreteSpaceGPU&>(out.getSpace());

    auto& inX = mySpace.getDeviceData();
    auto& outX = outSpace.getDeviceData();

    const Base::GPUFriendly &funcGPU = func.getGPUFriendlyVersion();

    funcGPU.GPUApply(inX, outX);

    return out;
}

DiscreteFunction&
R2toR::FunctionArbitraryGPU::Add(const R2toR::DiscrBase &toi) {
    getSpace().Add(toi.getSpace());

    return *this;
}

DiscreteFunction &
R2toR::FunctionArbitraryGPU::StoreAddition(const R2toR::DiscrBase &toi1,
                                           const R2toR::DiscrBase &toi2) {
    getSpace().StoreAddition(toi1.getSpace(), toi2.getSpace());

    return *this;
}

DiscreteFunction &
R2toR::FunctionArbitraryGPU::StoreSubtraction(const R2toR::DiscrBase &aoi1,
                                              const R2toR::DiscrBase &aoi2) {
    getSpace().StoreSubtraction(aoi1.getSpace(), aoi2.getSpace());

    return *this;
}

DiscreteFunction &R2toR::FunctionArbitraryGPU::Multiply(floatt a) {
    getSpace().Multiply(a);

    return *this;
}

R2toR::DiscrBase &
FunctionArbitraryGPU::operator +=(const MyBase &func) {


    if(helper==nullptr) helper = new FunctionArbitraryGPU(N, xMin, h);

    bool renderingOk = func.renderToDiscreteFunction(helper);
    if(renderingOk) this->Add(*helper);
    else {
        auto &space = getSpace();
        auto &XHost = space.getHostData(true);

        const floatt Lx = xMax - xMin;
        const floatt Ly = yMax - yMin;

        for (PosInt n = 0; n < N; n++) {
            for (PosInt m = 0; m < M; m++) {
                const floatt x = Lx * n / (N - 1) + xMin;
                const floatt y = Ly * m / (M - 1) + yMin;

                const Real2D r = {x, y};

                if (!func.domainContainsPoint(r))
                    continue;

                XHost[n + m * N] += func(r);
            }
        }

        space.upload();
    }

    return *this;
}



