//
// Created by joao on 17/04/2021.
//

#include "KG-RtoREnergyCalculator.h"

//#ifdef PERIODIC_BC
#define LEFT(i)  ((i)>0?(i)-1:N-1)
#define RIGHT(i) ((i)<N-1?(i)+1:0)
//#endif

RtoR::KGEnergy::KGEnergy(Core::Simulation::VoidBuilder &builder)
: builder(builder)
, _oEnergyDensityFunc(builder.NewFunctionArbitrary<RtoR::DiscreteFunction>())
, _oKinetic          (builder.NewFunctionArbitrary<RtoR::DiscreteFunction>())
, _oGradient         (builder.NewFunctionArbitrary<RtoR::DiscreteFunction>())
, _oPotential        (builder.NewFunctionArbitrary<RtoR::DiscreteFunction>())
{

}

auto RtoR::KGEnergy::computeDensities(const RtoR::EquationState &field) -> const RtoR::DiscreteFunction &{
    auto &phi = field.getPhi(), &ddtPhi = field.getDPhiDt();
    auto &phiSpace = phi.getSpace(),
            &ddtPhiSpace = ddtPhi.getSpace();

    RealArray &e = _oEnergyDensityFunc->getSpace().getHostData();
    RealArray &k = _oKinetic->getSpace().getHostData();
    RealArray &grad = _oGradient->getSpace().getHostData();
    RealArray &v = _oPotential->getSpace().getHostData();

    const floatt h = phiSpace.geth();
    const floatt inv2h = .5/h;
    auto &X = phiSpace.getHostData();
    auto &dXdt = ddtPhiSpace.getHostData();
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

auto RtoR::KGEnergy::integrateEnergy() -> Real
{
    RealArray &E_v = _oEnergyDensityFunc->getSpace().getHostData();
    Real E=0;

    for(const auto &e : E_v)
        E += e;

    Real dx = builder.getNumericParams().geth();
    return E*dx;
}



auto RtoR::KGEnergy::integrateEnergy(Real xmin, Real xmax) -> Real {
    auto &func = *_oEnergyDensityFunc;

    RealArray &E_v = _oEnergyDensityFunc->getSpace().getHostData();
    Real dx = builder.getNumericParams().geth();

    PosInt iMin = func.mapPosToInt(xmin), iMax = func.mapPosToInt(xmax);

    Real E=0;
    for(PosInt i=iMin; i<iMax; ++i)
        E += E_v[i];

    return E*dx;

    return 0;
}

Real RtoR::KGEnergy::integrateKinetic() {
    RealArray &K_v = _oKinetic->getSpace().getHostData();
    Real K=0;

    for(const auto &k : K_v)
        K += k;

    Real dx = builder.getNumericParams().geth();
    return K*dx;
}

Real RtoR::KGEnergy::integrateGradient() {
    RealArray &Grad_v = _oGradient->getSpace().getHostData();
    Real Grad=0;

    for(const auto &grad : Grad_v)
        Grad += grad;

    Real dx = builder.getNumericParams().geth();
    return Grad*dx;
}

Real RtoR::KGEnergy::integratePotential() {
    RealArray &Pot_v = _oPotential->getSpace().getHostData();
    Real Pot=0;

    for(const auto &pot : Pot_v)
        Pot += pot;

    Real dx = builder.getNumericParams().geth();
    return Pot*dx;
}
