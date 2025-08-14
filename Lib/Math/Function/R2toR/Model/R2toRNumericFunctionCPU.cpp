//
// Created by joao on 19/09/19.

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-use-auto"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
//

#include "R2toRNumericFunctionCPU.h"

#include "Utils/OMPUtils.h"

#define USE_DISCRETE_LAPLACE_KERNEL_BIG false

namespace Slab::Math::R2toR {

    NumericFunction_CPU::NumericFunction_CPU(UInt N, UInt M, DevFloat xMin, DevFloat yMin, DevFloat hx, DevFloat hy)
        : FNumericFunction(N, M, xMin, yMin, hx, hy, Device::CPU) {

    }

    TPointer<Base::NumericFunction<Real2D, DevFloat>> NumericFunction_CPU::CloneWithSize(UInt outN) const {
        assert(M==N); // (por enquanto so vai funcionar assim.

        auto myClone = DataAlloc<NumericFunction_CPU>(get_data_name() + " [clone]", outN, outN, xMin, yMin, hx, hy);;

        const DevFloat incX_d = N / DevFloat(outN);
        const DevFloat incY_d = M / DevFloat(outN);
        for(UInt n=0; n<outN; n++){
            for(UInt m=0; m<outN; m++){
                fix myn = UInt(n*incX_d);
                fix mym = UInt(m*incY_d);
                myClone->At(n,m) = At(myn,mym);
            }
        }

        return myClone;
    }

    const DevFloat&
    NumericFunction_CPU::At(UInt n, UInt m) const {
        // assert(n<N && m<M);
        n %= N;
        m %= M;

        return getSpace().getHostData()[n + m * N];
    }

    DevFloat &NumericFunction_CPU::At(UInt n, UInt m) {
        // assert(n<N && m<M);
        n %= N;
        m %= M;
        return getSpace().getHostData()[n + m * N];
    }

    FNumericFunction &NumericFunction_CPU::Laplacian(FNumericFunction &outFunc) const {
        if(!Common::AreEqual(hx, hy)) throw "No computation of discrete laplacian for dx!=dy";

        fix h = hx;
        const DevFloat invhsqr = 1./(h*h);

        /*for(UInt n=0; n<N; n++){
            outFunc.At(n,0) = 0.0;
            outFunc.At(n,M-1) = 0.0;
        }
        for(UInt m=1; m<M-1; m++){ // de 1 ate M-2 porque o 0 e o M-1 ja foram feitos acima.
            outFunc.At(0,m) = 0.0;
            outFunc.At(N-1,m) = 0.0;
        }*/

        OMP_GET_BEGIN_END(begin, end, N)
        begin = be.first==0?1:be.first;
        end = be.second==N?N-1:be.second;

        for(UInt n=begin; n<end; n++)
        {
            for (UInt m = 1; m < M - 1; m++) {
    #if USE_DISCRETE_LAPLACE_KERNEL_BIG
                const DevFloat dx = At(n - 1, m) + At(n + 1, m);
                const DevFloat dy = At(n, m - 1) + At(n, m + 1);
                const DevFloat dxy = At(n-1, m-1) + At(n+1, m+1);
                const DevFloat dyx = At(n-1, m+1) + At(n+1, m-1);
                outFunc.At(n, m) = invhsqr * (.5*(dx+dy) + .25*(dxy+dyx) - 3. * At(n, m));
    #else
                const DevFloat dx = At(n - 1, m) + At(n + 1, m);
                const DevFloat dy = At(n, m - 1) + At(n, m + 1);

                OUT out = outFunc.At(n, m);
                fix dϕ = invhsqr * ((dx + dy) - 4. * At(n, m));
                out = dϕ;
    #endif
            }
        }

        return outFunc;
    }

    NumericFunction_CPU &NumericFunction_CPU::Set(const R2toR::Function &func) {
        const DevFloat L1 = xMax-xMin;
        const DevFloat L2 = yMax-yMin;

        OMP_GET_BEGIN_END(begin, end, N)
        for(UInt n=begin; n<end; n++)
            for(UInt m=0; m<M; m++) {
                Real2D x = {L1 * n / (N - 1) + xMin, L2 * m / (M - 1) + yMin};

                if(!func.domainContainsPoint(x)) continue;

                this->At(n, m) = func(x);
            }

        return *this;
    }


    //Base::ArbitraryFunction<Real2D, DevFloat> &FunctionArbitraryCPU::SetArb(
    //        const Base::ArbitraryFunction<Real2D, DevFloat> &function) {
    //    cast(func, const ArbitraryFunction&, function)
    //
    //    OMP_GET_BEGIN_END(begin, end, N)
    //    for(UInt n=begin; n<end; n++)
    //        for(UInt m=0; m<M; m++)
    //            At(n, m) = func.At(n,m);
    //
    //    return *this;
    //}


    Base::NumericFunction<Real2D, DevFloat> &
    NumericFunction_CPU::Apply(const FunctionT<DevFloat, DevFloat> &func,
                                Base::NumericFunction<Real2D, DevFloat> &out) const {
        SlabCast(fOut, FNumericFunction &, out)

        OMP_GET_BEGIN_END(begin, end, N)
        for(UInt n=begin; n<end; n++)
            for(UInt m=0; m<M; m++)
                fOut.At(n, m) = func(At(n,m));

        return out;
    }

    Function_ptr NumericFunction_CPU::Clone() const {
        return DataAlloc<NumericFunction_CPU> (this->get_data_name() + " [clone]", *this);
    }

}


#pragma clang diagnostic pop