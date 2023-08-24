//

#include "R2toRDiscreteFunction.h"

// don't touch:
#define PERIODIC 1
#define OD_NAN 2
#define OD_ZERO 3

// ok touch:
#define OFF_DOMAIN_BEHAVIOR PERIODIC

//
// Created by joao on 30/09/2019.
R2toR::DiscreteFunction::DiscreteFunction(PosInt N, PosInt M, Real xMin, Real yMin, Real h, device dev)
: DiscreteFunctionBase(DimensionMetaData({N, M}), h, dev)
, N(N), M(M)
, xMin(xMin), xMax(xMin+(Real)N*h)
, yMin(yMin), yMax(yMin+(Real)M*h)
, h(h)
{ }

R2toR::DiscreteFunction::DiscreteFunction(const NumericConfig &p, device dev)
: R2toR::DiscreteFunction(p.getN(), p.getN(), p.getxMin(), p.getxMin(), p.geth(), dev)
{ }

Real R2toR::DiscreteFunction::operator()(Real2D x) const {
    const Real Lx = xMax-xMin;
    const Real Ly = yMax-yMin;
    const Real d = h;
    int n = int((N-1) * (d+x.x-xMin)/Lx);
    int m = int((M-1) * (d+x.y-yMin)/Ly);

    // TODO: fazer uma macro para colocar o que esta na linha logo abaixo, de forma que no modo Release isso
    //  nao seja incluido no codigo.

    #if OFF_DOMAIN_BEHAVIOR == OD_NAN
    if(n<0 || m<0 || n>N-1 || m>M-1) return NaN;
    #elif OFF_DOMAIN_BEHAVIOR == OD_ZERO
    if(n<0 || m<0 || n>N-1 || m>M-1) return OD_ZERO;
    #elif OFF_DOMAIN_BEHAVIOR == PERIODIC
    if (n < 0) n = N - abs(n) % N;
    else if (n > N - 1) n = n % N;
    if (m < 0) m = M - abs(m) % M;
    else if (m > M - 1) m = m % M;
    #endif

    return At(n,m);
}

auto R2toR::DiscreteFunction::operator()(Real2D x) -> Real& {
    const Real Lx = xMax-xMin;
    const Real Ly = yMax-yMin;
    const Real d = h;
    int n = int(floor(Real(N-1) * (d+x.x-xMin)/Lx));
    int m = int(floor(Real(M-1) * (d+x.y-yMin)/Ly));

    return At(n,m);
}

PosInt R2toR::DiscreteFunction::getN() const { return N; }

PosInt R2toR::DiscreteFunction::getM() const { return M; }

R2toR::Domain R2toR::DiscreteFunction::getDomain() const { return {xMin, xMax, yMin, yMax}; }

Real R2toR::DiscreteFunction::diff(int dim, Real2D x) const {
    const Real Lx = xMax-xMin;
    const Real Ly = yMax-yMin;
    int n = int((N-1)*(x.x-xMin)/Lx);
    int m = int((M-1)*(x.y-yMin)/Ly);

    if(n<1 || n>N-1 || m<1 || m>M-1) return .0;

    getSpace().syncHost();
    auto& X = getSpace().getHostData();
    int j = n + m*N;

    const Real inv2h = .5/h;
    if(dim == 0){
        return inv2h * (At(n+1, m) - At(n-1, m));
    } else if (dim == 1){
        return inv2h * (At(n, m+1) - At(n, m-1));
    } else throw "Tidak bagus diff.";
}

Base::FunctionT<Real2D, Real>::Ptr R2toR::DiscreteFunction::diff(int n) const {
    throw "R2toR::FunctionArbitrary::diff(int n) not implemented";
    return FunctionT::diff(n);
}

Str R2toR::DiscreteFunction::myName() const
{ return Base::DiscreteFunction<Real2D, Real>::myName() + " 2D " + (dev==GPU ? "GPU" : "CPU"); }
