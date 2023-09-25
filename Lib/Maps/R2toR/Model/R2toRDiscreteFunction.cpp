//

#include "R2toRDiscreteFunction.h"

// don't touch:
#define PERIODIC 1
#define OD_NAN 2
#define OD_ZERO 3
#define ROUND_ROUND(x) round(x)
#define FLOOR_ROUND(x) floor(.5f + (x))

// ok touch:
#define OFF_DOMAIN_BEHAVIOR PERIODIC
#define ROUND(x) ROUND_ROUND((x))

//
// Created by joao on 30/09/2019.
R2toR::DiscreteFunction::DiscreteFunction(UInt N, UInt M, Real xMin, Real yMin, Real hx, Real hy, device dev)
: DiscreteFunctionBase(DimensionMetaData({N, M}, {hx, hy}), dev)
, N(N), M(M)
, xMin(xMin), xMax(xMin+(Real)N*hx)
, yMin(yMin), yMax(yMin+(Real)M*hy)
, hx(hx)
, hy(hy)
{ }

R2toR::DiscreteFunction::DiscreteFunction(const NumericConfig &p, device dev)
: R2toR::DiscreteFunction(p.getN(), p.getN(), p.getxMin(), p.getxMin(), p.geth(), p.geth(), dev)
{ }

Real R2toR::DiscreteFunction::operator()(Real2D r) const {
    const Real Lx = xMax-xMin;
    const Real Ly = yMax-yMin;
    fix x=r.x;
    fix y=r.y;

    fix nReal = (N-1) * (x-xMin)/Lx;
    fix mReal = (M-1) * (y-yMin)/Ly;

    int n = ROUND(nReal);
    int m = ROUND(mReal);


    // TODO: responsabilizar classes filhas por acesso fora do dominio.

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

auto R2toR::DiscreteFunction::operator()(Real2D r) -> Real& {
    const Real Lx = xMax-xMin;
    const Real Ly = yMax-yMin;
    fix x=r.x;
    fix y=r.y;

    fix nReal = (N-1) * (x-xMin)/Lx;
    fix mReal = (M-1) * (y-yMin)/Ly;

    int n = ROUND(nReal);
    int m = ROUND(mReal);

    return At(n,m);
}

UInt R2toR::DiscreteFunction::getN() const { return N; }

UInt R2toR::DiscreteFunction::getM() const { return M; }

R2toR::Domain R2toR::DiscreteFunction::getDomain() const { return {xMin, xMax, yMin, yMax}; }

Real R2toR::DiscreteFunction::diff(int dim, Real2D x) const {
    const Real Lx = xMax-xMin;
    const Real Ly = yMax-yMin;
    int n = int((N-1)*(x.x-xMin)/Lx);
    int m = int((M-1)*(x.y-yMin)/Ly);

    if(n<1 || n>N-1 || m<1 || m>M-1) return .0;

    getSpace().syncHost();
    auto& X = getSpace().getHostData();
    // int j = n + m*N;

    if(dim == 0){
        const Real inv2h = .5/hx;
        return inv2h * (At(n+1, m) - At(n-1, m));
    } else if (dim == 1){
        const Real inv2h = .5/hy;
        return inv2h * (At(n, m+1) - At(n, m-1));
    } else NOT_IMPLEMENTED;
}

Core::FunctionT<Real2D, Real>::Ptr R2toR::DiscreteFunction::diff(int n) const {
    NOT_IMPLEMENTED
}

Str R2toR::DiscreteFunction::myName() const
{ return Core::DiscreteFunction<Real2D, Real>::myName() + " 2D " + (dev == GPU ? "GPU" : "CPU"); }

bool R2toR::DiscreteFunction::domainContainsPoint(Real2D x) const {
    return x.x>=xMin && x.x<=xMax && x.y>=yMin && x.y<=yMax;
}
