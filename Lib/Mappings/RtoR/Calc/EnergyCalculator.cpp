//
// Created by joao on 17/04/2021.
//

#include "EnergyCalculator.h"

#ifdef PERIODIC_BC
#define LEFT(i)  ((i)>0?(i)-1:N-1)
#define RIGHT(i) ((i)<N-1?(i)+1:0)
#endif

auto RtoR::EnergyCalculator::computeDensities(const RtoR::FieldState &field) -> const RtoR::ArbitraryFunction &{
    auto &phi = field.getPhi(), &ddtPhi = field.getDPhiDt();
    auto &phiSpace = phi.getSpace(),
            &ddtPhiSpace = ddtPhi.getSpace();

    VecFloat &e = _oEnergyDensityFunc->getSpace().getX();
    VecFloat &k = _oKinetic->getSpace().getX();
    VecFloat &grad = _oGradient->getSpace().getX();
    VecFloat &v = _oPotential->getSpace().getX();

    const floatt h = phiSpace.geth();
    const floatt inv2h = .5/h;
    auto &X = phiSpace.getX();
    auto &dXdt = ddtPhiSpace.getX();
    const int N = X.size();
    {
        for (int i = 0; i < N; i++) {
            const Real xl = X[LEFT(i)], xcAbs = fabs(X[i]),
                       xr = X[RIGHT(i)],
                       dxdt = dXdt[i];

            const Real dxdx = inv2h*(xr-xl);

            k[i] = .5*dxdt*dxdt;
            grad[i] = .5*dxdx*dxdx;
            v[i] = xcAbs;

            e[i] = k[i] + grad[i] + v[i];
        }
    }

    return *_oEnergyDensityFunc;
}

auto RtoR::EnergyCalculator::integrateEnergy() -> Real
{
    VecFloat &E_v = _oEnergyDensityFunc->getSpace().getX();
    Real E=0;

    for(const auto &e : E_v)
        E += e;

    Real dx = Numerics::Allocator::getInstance().getNumericParams().geth();
    return E*dx;
}



auto RtoR::EnergyCalculator::integrateEnergy(Real xmin, Real xmax) -> Real {
    auto &func = *_oEnergyDensityFunc;

    VecFloat &E_v = _oEnergyDensityFunc->getSpace().getX();
    Real dx = Numerics::Allocator::getInstance().getNumericParams().geth();

    PosInt iMin = func.mapPosToInt(xmin), iMax = func.mapPosToInt(xmax);

    Real E=0;
    for(PosInt i=iMin; i<iMax; ++i)
        E += E_v[i];

    return E*dx;

    return 0;
}

Real RtoR::EnergyCalculator::integrateKinetic() {
    VecFloat &K_v = _oKinetic->getSpace().getX();
    Real K=0;

    for(const auto &k : K_v)
        K += k;

    Real dx = Numerics::Allocator::getInstance().getNumericParams().geth();
    return K*dx;
}

Real RtoR::EnergyCalculator::integrateGradient() {
    VecFloat &Grad_v = _oGradient->getSpace().getX();
    Real Grad=0;

    for(const auto &grad : Grad_v)
        Grad += grad;

    Real dx = Numerics::Allocator::getInstance().getNumericParams().geth();
    return Grad*dx;
}

Real RtoR::EnergyCalculator::integratePotential() {
    VecFloat &Pot_v = _oPotential->getSpace().getX();
    Real Pot=0;

    for(const auto &pot : Pot_v)
        Pot += pot;

    Real dx = Numerics::Allocator::getInstance().getNumericParams().geth();
    return Pot*dx;
}
