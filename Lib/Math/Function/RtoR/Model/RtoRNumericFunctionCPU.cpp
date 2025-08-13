#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
#pragma ide diagnostic ignored "modernize-use-auto"
#pragma ide diagnostic ignored "hicpp-use-auto"


#include "RtoRNumericFunctionCPU.h"
#include "Math/Function/RtoR/Model/Operators/DerivativesCPU.h"

#include "Utils/OMPUtils.h"
#include "Math/Data/DataAllocator.h"

// Created by joao on 17/09/2019.


namespace Slab::Math::RtoR {

#define PERIODIC_BORDER true
#define FIXED_BORDER false

    NumericFunction_CPU::NumericFunction_CPU(const NumericFunction_CPU &toCopy)
            : NumericFunction(toCopy.N, toCopy.xMin, toCopy.xMax, Device::CPU, toCopy.laplacianType) {

        auto &toCopyX = toCopy.getSpace().getHostData(true);

        for (UInt n = 0; n < N; n++)
            getSpace().getHostData()[n] = toCopyX[n];
    }

    NumericFunction_CPU::NumericFunction_CPU(const NumericFunction &toCopy)
            : NumericFunction(toCopy.N, toCopy.xMin, toCopy.xMax, Device::CPU, toCopy.getLaplacianType()) {

        auto &toCopyX = toCopy.getSpace().getHostData(true);

        for (UInt n = 0; n < N; n++)
            getSpace().getHostData()[n] = toCopyX[n];
    }

    RtoR::NumericFunction_CPU::NumericFunction_CPU(UInt N, DevFloat xLeft, DevFloat xRight, LaplacianType laplacianType)
            : NumericFunction(N, xLeft, xRight, Device::CPU, laplacianType) {

    }

    NumericFunction_CPU::NumericFunction_CPU(RealArray_I &data, DevFloat xLeft, DevFloat xRight,
                                               NumericFunction::LaplacianType laplacianType)
            : NumericFunction_CPU(data.size(), xLeft, xRight, laplacianType) {
        getSpace().getHostData() = data;
    }

    auto NumericFunction_CPU::Set(const RealArray &vec) -> NumericFunction_CPU & {
        assert(vec.size() == getSpace().getHostData().size());

        for (int i = 0; i < vec.size(); ++i)
            this->getSpace().getHostData()[i] = vec[i];

        return *this;
    }

    NumericFunction_CPU &NumericFunction_CPU::Set(const FunctionT &function) {
        const floatt L = xMax - xMin;
        RealArray &X = getSpace().getHostData();

        fix dx = L / N;
        OMP_PARALLEL_FOR(i, N) {
            const floatt x = i * dx + xMin;

            X[i] = function(x);
        }

        return *this;
    }

    Function_ptr NumericFunction_CPU::Clone() const {
        return DataAlloc<NumericFunction_CPU> (this->get_data_name() + " [clone]", *this);
    }

    TPointer<Base::NumericFunction <DevFloat, DevFloat>> NumericFunction_CPU::CloneWithSize(UInt outN) const {
        auto newFunc = DataAlloc<NumericFunction_CPU>(this->get_data_name() + "[decimated " + ToStr(outN) + "/" + ToStr(this->N) + " clone]", outN, xMin, xMax, laplacianType);

        const RealArray &X = getSpace().getHostData();
        const DevFloat inc_d = N / DevFloat(outN);

        DiscreteSpace &newSpace = newFunc->getSpace();
        for (UInt i = 0; i < outN; i++)
            newSpace.getHostData()[i] = X[int(i * inc_d)];;

        return newFunc;
    }

    Base::NumericFunction <DevFloat, DevFloat> &NumericFunction_CPU::Apply(const FunctionT &func,
                                                                    Base::NumericFunction <DevFloat, DevFloat> &out) const {
        auto &outSpace = out.getSpace();

        auto &outX = outSpace.getHostData();
        const auto &myX = getSpace().getHostData();

        OMP_PARALLEL_FOR(n, N)outX[n] = func(myX[n]);

        return out;
    }

    NumericFunction &NumericFunction_CPU::Laplacian(NumericFunction &out) const {

        if (laplacianType == RadialSymmetry2D) {
            auto &outX = out.getSpace().getHostData();
            const auto &myX = getSpace().getHostData();
            const auto &f = this->getSpace().getHostData();
            const DevFloat invh = 1. / getSpace().getMetaData().geth(0);
            const DevFloat invh_2 = .5 * invh;
            const DevFloat invhsqr = invh * invh;

            OMP_GET_BEGIN_END(begin, end, outX.size())

            for (int i = begin; i < end; i++) {
                if (i == 0 || i == outX.size() - 1) {
                    outX[i] = .0;
                    continue;
                }

                const DevFloat r = mapIntToPos(i);
                const DevFloat ddr = invh_2 * (-f[i - 1] + f[i + 1]);
                const DevFloat d2dr2 = invhsqr * ((f[i - 1] + f[i + 1]) - 2.0 * f[i]);

                outX[i] = d2dr2 + ddr / r;
                //outX[i] = d2dr2;
            }
        } else if (laplacianType == Standard1D_FixedBorder || laplacianType == Standard1D_PeriodicBorder) {
            DerivativeCPU deriv(*this);
            deriv.d2fdx2_v(out.getSpace().getHostData());
        } else throw "Unknown Laplacian type";

        return out;
    }

    DevFloat NumericFunction_CPU::integrate() const {

        auto sum = .0;
        for (const auto &v: this->getSpace().getHostData())
            sum += v;

        auto dx = (this->xMax - this->xMin) / this->N;
        return sum * dx;
    }

    Base::NumericFunction <DevFloat, DevFloat> &NumericFunction_CPU::operator+=(const FunctionT<DevFloat, DevFloat> &func) {
        auto &outSpace = getSpace();

        auto &outX = outSpace.getHostData();
        const auto &myX = getSpace().getHostData();

        OMP_PARALLEL_FOR(n, N) {
            const auto x = mapIntToPos(n);
            outX[n] += func(x);
        }

        return *this;
    }

    DevFloat NumericFunction_CPU::max() const {
        return getSpace().getHostData().max();
    }

    DevFloat NumericFunction_CPU::min() const {
        return getSpace().getHostData().min();
    }

}

#pragma clang diagnostic pop