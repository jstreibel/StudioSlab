


#include "R2toRNumericFunctionGPU.h"

#include "Math/Function/R2toR/Model/Operators/DerivativesGPU.h"
#include "Math/Function/GPUFriendly.h"



namespace Slab::Math::R2toR {


    NumericFunction_GPU::NumericFunction_GPU(UInt N, UInt M, DevFloat xMin, DevFloat yMin, DevFloat hx, DevFloat hy)
            : R2toR::FNumericFunction(N, M, xMin, yMin, hx, hy, GPU) {

    }

    NumericFunction_GPU::NumericFunction_GPU(UInt N, DevFloat sMin, DevFloat h)
            : NumericFunction_GPU(N, N, sMin, sMin, h, h) {

    }

    NumericFunction_GPU::~NumericFunction_GPU() {
        delete helper;
    }

    R2toR::FNumericFunction &R2toR::NumericFunction_GPU::Laplacian(R2toR::FNumericFunction &outFunc) const {
        // assert(getSpace().isCompatible(outFunc.getSpace()))

        SlabCast(inSpace, const DiscreteSpaceGPU&, getSpace())
        SlabCast(outSpace, DiscreteSpaceGPU & , outFunc.getSpace())


        d2dx2(inSpace.getDeviceData(), outSpace.getDeviceData(), inSpace.getMetaData().geth(0), N, M);

        return outFunc;
    }

//
// Created by joao on 27/09/2019.
    TPointer<Base::FunctionT <Real2D, DevFloat>> R2toR::NumericFunction_GPU::Clone() const {
        auto &h = getSpace().getMetaData().geth();
        return New<NumericFunction_GPU>(N, M, xMin, yMin, h[0], h[1]);
    }

    TPointer<Base::NumericFunction<Real2D, DevFloat>> R2toR::NumericFunction_GPU::CloneWithSize(UInt N) const {
        NOT_IMPLEMENTED_CLASS_METHOD
    }

    const DevFloat &R2toR::NumericFunction_GPU::At(UInt n, UInt m) const {
        assert(n < N && m < M);

        return getSpace().getHostData()[n + m * N];
    }

    DevFloat &R2toR::NumericFunction_GPU::At(UInt n, UInt m) {
        assert(n < N && m < M);

        return getSpace().getHostData()[n + m * N];
    }

    FNumericFunction &
    R2toR::NumericFunction_GPU::Set(const MyBase &func) {
        auto &space = getSpace();

        if (func.isDiscrete()) {
            auto &discreteFunc = dynamic_cast<const R2toR::FNumericFunction &>(func);
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

                if (!func.domainContainsPoint(r))
                    continue;

                XHost[n + m * N] = func(r);
            }
        }

        space.upload();

        return *this;
    }

    FNumericFunction &
    R2toR::NumericFunction_GPU::SetArb(const Base::NumericFunction <Real2D, DevFloat> &func) {
        getSpace().setToValue(func.getSpace());

        return *this;
    }

    Base::NumericFunction <Real2D, DevFloat> &NumericFunction_GPU::Apply(const Base::FunctionT <DevFloat, DevFloat> &func,
                                                                     Base::NumericFunction <Real2D, DevFloat> &out) const {
        assert(func.isGPUFriendly());

        const auto &mySpace = dynamic_cast<const DiscreteSpaceGPU &>(getSpace());
        auto &outSpace = dynamic_cast<DiscreteSpaceGPU &>(out.getSpace());

        auto &inX = mySpace.getDeviceData();
        auto &outX = outSpace.getDeviceData();

        const Base::GPUFriendly &funcGPU = func.getGPUFriendlyVersion();

        funcGPU.GPUApply(inX, outX);

        return out;
    }

    FNumericFunction &
    R2toR::NumericFunction_GPU::Add(const R2toR::DiscrBase &toi) {
        getSpace().Add(toi.getSpace());

        return *this;
    }

    FNumericFunction &
    R2toR::NumericFunction_GPU::StoreAddition(const R2toR::DiscrBase &toi1,
                                              const R2toR::DiscrBase &toi2) {
        getSpace().StoreAddition(toi1.getSpace(), toi2.getSpace());

        return *this;
    }

    FNumericFunction &
    R2toR::NumericFunction_GPU::StoreSubtraction(const R2toR::DiscrBase &aoi1,
                                                 const R2toR::DiscrBase &aoi2) {
        getSpace().StoreSubtraction(aoi1.getSpace(), aoi2.getSpace());

        return *this;
    }

    FNumericFunction &R2toR::NumericFunction_GPU::Multiply(floatt a) {
        getSpace().Multiply(a);

        return *this;
    }

    R2toR::DiscrBase &
    NumericFunction_GPU::operator+=(const MyBase &func) {

        if (helper == nullptr) helper = new NumericFunction_GPU(N, xMin, getSpace().getMetaData().geth(0));

        bool renderingOk = func.renderToNumericFunction(helper);
        if (renderingOk) this->Add(*helper);
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


}