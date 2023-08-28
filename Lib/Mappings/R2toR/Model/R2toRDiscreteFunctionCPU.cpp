#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-use-auto"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
//

#include "R2toRDiscreteFunctionCPU.h"

#include "Utils/OMPUtils.h"

using namespace R2toR;

#define USE_DISCRETE_LAPLACE_KERNEL_BIG true

//
// Created by joao on 19/09/19.
DiscreteFunction_CPU::DiscreteFunction_CPU(PosInt N, PosInt M, Real xMin, Real yMin, Real h)
    : DiscreteFunction(N, M, xMin, yMin, h, device::CPU) {

}

Base::DiscreteFunction<Real2D, Real> *DiscreteFunction_CPU::CloneWithSize(PosInt outN) const {
    assert(M==N); // (por enquanto so vai funcionar assim.

    DiscreteFunction_CPU &myClone = *new DiscreteFunction_CPU(outN, outN, xMin, yMin, h);;

    const Real incX_d = N / Real(outN);
    const Real incY_d = M / Real(outN);
    for(PosInt n=0; n<outN; n++){
        for(PosInt m=0; m<outN; m++){
            const PosInt myn = n*incX_d;
            const PosInt mym = m*incY_d;
            myClone.At(n,m) = At(myn,mym);
        }
    }

    return &myClone;
}

Real DiscreteFunction_CPU::At(PosInt n, PosInt m) const {
    assert(n + m*N < N*M);

    return getSpace().getHostData()[n + m * N];
}

Real &DiscreteFunction_CPU::At(PosInt n, PosInt m) {
    assert(n + m*N < N*M);
    return getSpace().getHostData()[n + m * N];
}

DiscreteFunction &DiscreteFunction_CPU::Laplacian(DiscreteFunction &outFunc) const {
    const Real invhsqr = 1./(h*h);

    /*for(PosInt n=0; n<N; n++){
        outFunc.At(n,0) = 0.0;
        outFunc.At(n,M-1) = 0.0;
    }
    for(PosInt m=1; m<M-1; m++){ // de 1 ate M-2 porque o 0 e o M-1 ja foram feitos acima.
        outFunc.At(0,m) = 0.0;
        outFunc.At(N-1,m) = 0.0;
    }*/

    OMP_GET_BEGIN_END(begin, end, N)
    begin = be.first==0?1:be.first;
    end = be.second==N?N-1:be.second;

    for(PosInt n=begin; n<end; n++)
    {
        for (PosInt m = 1; m < M - 1; m++) {
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

DiscreteFunction_CPU &DiscreteFunction_CPU::Set(const R2toR::Function &func) {
    const Real L1 = xMax-xMin;
    const Real L2 = yMax-yMin;

    OMP_GET_BEGIN_END(begin, end, N)
    for(PosInt n=begin; n<end; n++)
        for(PosInt m=0; m<M; m++) {
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
//    for(PosInt n=begin; n<end; n++)
//        for(PosInt m=0; m<M; m++)
//            At(n, m) = func.At(n,m);
//
//    return *this;
//}


Base::DiscreteFunction<Real2D, Real> &
DiscreteFunction_CPU::Apply(const FunctionT<Real, Real> &func,
                            Base::DiscreteFunction<Real2D, Real> &out) const {
    cast(fOut, DiscreteFunction &, out)

    OMP_GET_BEGIN_END(begin, end, N)
    for(PosInt n=begin; n<end; n++)
        for(PosInt m=0; m<M; m++)
            fOut.At(n, m) = func(At(n,m));

    return out;
}

#pragma clang diagnostic pop