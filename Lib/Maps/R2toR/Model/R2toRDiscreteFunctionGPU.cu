


#include "R2toRDiscreteFunctionGPU.h"

#include "Maps/R2toR/Model/Derivatives/DerivativesGPU.h"
#include "Math/Function/GPUFriendly.h"



using namespace R2toR;


DiscreteFunction_GPU::DiscreteFunction_GPU(UInt N, UInt M, Real xMin, Real yMin, Real h)
: R2toR::DiscreteFunction(N, M, xMin, yMin, h, GPU)
{

}

DiscreteFunction_GPU::DiscreteFunction_GPU(UInt N, Real sMin, Real h)
: DiscreteFunction_GPU(N, N, sMin, sMin, h)
{

}

DiscreteFunction_GPU::~DiscreteFunction_GPU() {
    delete helper;
}

R2toR::DiscreteFunction &R2toR::DiscreteFunction_GPU::Laplacian(R2toR::DiscreteFunction &outFunc) const {
    // assert(getSpace().isCompatible(outFunc.getSpace()))

    cast(inSpace, const DiscreteSpaceGPU&, getSpace())
    cast(outSpace, DiscreteSpaceGPU&, outFunc.getSpace())

    d2dx2(inSpace.getDeviceData(), outSpace.getDeviceData(), inSpace.geth(), N, M);

    return outFunc;
}

//
// Created by joao on 27/09/2019.
Core::FunctionT<Real2D, Real> *R2toR::DiscreteFunction_GPU::Clone() const {
    return new DiscreteFunction_GPU(N, M, xMin, yMin, h);
}

Core::DiscreteFunction<Real2D, Real> *R2toR::DiscreteFunction_GPU::CloneWithSize(UInt N) const {
    throw "R2toRMap::FunctionArbitraryGPU::Clone(unsigned int) not implemented.";
}

Real R2toR::DiscreteFunction_GPU::At(UInt n, UInt m) const {
    assert(n<N && m<M);

    return getSpace().getHostData()[n + m * N];
}

Real &R2toR::DiscreteFunction_GPU::At(UInt n, UInt m) {
    assert(n<N && m<M);

    return getSpace().getHostData()[n + m * N];
}

DiscreteFunction &
R2toR::DiscreteFunction_GPU::Set(const MyBase &func) {
    auto &space = getSpace();

    if(func.isDiscrete()) {
        auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(func);
        return SetArb(discreteFunc);
    }

    space.syncHost();

    auto &XHost = space.getHostData(false);

    const floatt Lx = xMax - xMin;
    const floatt Ly = yMax - yMin;
    for (UInt n = 0; n < N; n++) {
        for (UInt m = 0; m < M; m++) {
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
R2toR::DiscreteFunction_GPU::SetArb(const Core::DiscreteFunction<Real2D, Real> &func) {
    getSpace().setToValue(func.getSpace());

    return *this;
}

Core::DiscreteFunction<Real2D, Real> &DiscreteFunction_GPU::Apply(const FunctionT<Real, Real> &func,
                                                                  Core::DiscreteFunction<Real2D, Real> &out) const {
    assert(func.isGPUFriendly());

    const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU&>(getSpace());
    auto &outSpace = dynamic_cast<DiscreteSpaceGPU&>(out.getSpace());

    auto& inX = mySpace.getDeviceData();
    auto& outX = outSpace.getDeviceData();

    const Core::GPUFriendly &funcGPU = func.getGPUFriendlyVersion();

    funcGPU.GPUApply(inX, outX);

    return out;
}

DiscreteFunction&
R2toR::DiscreteFunction_GPU::Add(const R2toR::DiscrBase &toi) {
    getSpace().Add(toi.getSpace());

    return *this;
}

DiscreteFunction &
R2toR::DiscreteFunction_GPU::StoreAddition(const R2toR::DiscrBase &toi1,
                                           const R2toR::DiscrBase &toi2) {
    getSpace().StoreAddition(toi1.getSpace(), toi2.getSpace());

    return *this;
}

DiscreteFunction &
R2toR::DiscreteFunction_GPU::StoreSubtraction(const R2toR::DiscrBase &aoi1,
                                              const R2toR::DiscrBase &aoi2) {
    getSpace().StoreSubtraction(aoi1.getSpace(), aoi2.getSpace());

    return *this;
}

DiscreteFunction &R2toR::DiscreteFunction_GPU::Multiply(floatt a) {
    getSpace().Multiply(a);

    return *this;
}

R2toR::DiscrBase &
DiscreteFunction_GPU::operator +=(const MyBase &func) {


    if(helper==nullptr) helper = new DiscreteFunction_GPU(N, xMin, h);

    bool renderingOk = func.renderToDiscreteFunction(helper);
    if(renderingOk) this->Add(*helper);
    else {
        auto &space = getSpace();
        auto &XHost = space.getHostData(true);

        const floatt Lx = xMax - xMin;
        const floatt Ly = yMax - yMin;

        for (UInt n = 0; n < N; n++) {
            for (UInt m = 0; m < M; m++) {
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




