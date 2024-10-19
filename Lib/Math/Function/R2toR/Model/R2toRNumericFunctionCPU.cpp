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

    NumericFunction_CPU::NumericFunction_CPU(UInt N, UInt M, Real xMin, Real yMin, Real hx, Real hy)
        : NumericFunction(N, M, xMin, yMin, hx, hy, Device::CPU) {

    }

    Pointer<Base::NumericFunction<Real2D, Real>> NumericFunction_CPU::CloneWithSize(UInt outN) const {
        assert(M==N); // (por enquanto so vai funcionar assim.

        auto myClone = DataAlloc<NumericFunction_CPU>(get_data_name() + " [clone]", outN, outN, xMin, yMin, hx, hy);;

        const Real incX_d = N / Real(outN);
        const Real incY_d = M / Real(outN);
        for(UInt n=0; n<outN; n++){
            for(UInt m=0; m<outN; m++){
                const UInt myn = n*incX_d;
                const UInt mym = m*incY_d;
                myClone->At(n,m) = At(myn,mym);
            }
        }

        return myClone;
    }

    const Real&
    NumericFunction_CPU::At(UInt n, UInt m) const {
        // assert(n<N && m<M);
        n %= N;
        m %= M;

        return getSpace().getHostData()[n + m * N];
    }

    Real &NumericFunction_CPU::At(UInt n, UInt m) {
        // assert(n<N && m<M);
        n %= N;
        m %= M;
        return getSpace().getHostData()[n + m * N];
    }

    NumericFunction &NumericFunction_CPU::Laplacian(NumericFunction &outFunc) const {
        if(!Common::AreEqual(hx, hy)) throw "No computation of discrete laplacian for dx!=dy";

        fix h = hx;
        const Real invhsqr = 1./(h*h);

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
                const Real dx = At(n - 1, m) + At(n + 1, m);
                const Real dy = At(n, m - 1) + At(n, m + 1);
                const Real dxy = At(n-1, m-1) + At(n+1, m+1);
                const Real dyx = At(n-1, m+1) + At(n+1, m-1);
                outFunc.At(n, m) = invhsqr * (.5*(dx+dy) + .25*(dxy+dyx) - 3. * At(n, m));
    #else
                const Real dx = At(n - 1, m) + At(n + 1, m);
                const Real dy = At(n, m - 1) + At(n, m + 1);
                outFunc.At(n, m) = invhsqr * ((dx + dy) - 4. * At(n, m));
    #endif
            }
        }

        return outFunc;
    }

    NumericFunction_CPU &NumericFunction_CPU::Set(const R2toR::Function &func) {
        const Real L1 = xMax-xMin;
        const Real L2 = yMax-yMin;

        OMP_GET_BEGIN_END(begin, end, N)
        for(UInt n=begin; n<end; n++)
            for(UInt m=0; m<M; m++) {
                Real2D x = {L1 * n / (N - 1) + xMin, L2 * m / (M - 1) + yMin};

                if(!func.domainContainsPoint(x)) continue;

                this->At(n, m) = func(x);
            }

        return *this;
    }


    //Base::ArbitraryFunction<Real2D, Real> &FunctionArbitraryCPU::SetArb(
    //        const Base::ArbitraryFunction<Real2D, Real> &function) {
    //    cast(func, const ArbitraryFunction&, function)
    //
    //    OMP_GET_BEGIN_END(begin, end, N)
    //    for(UInt n=begin; n<end; n++)
    //        for(UInt m=0; m<M; m++)
    //            At(n, m) = func.At(n,m);
    //
    //    return *this;
    //}


    Base::NumericFunction<Real2D, Real> &
    NumericFunction_CPU::Apply(const FunctionT<Real, Real> &func,
                                Base::NumericFunction<Real2D, Real> &out) const {
        SlabCast(fOut, NumericFunction &, out)

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