//
// Created by joao on 17/04/2021.
//

#include "KG-RtoREnergyCalculator.h"

#ifdef PERIODIC_BC
#undef PERIODIC_BC
#endif

#ifdef PERIODIC_BC
#define BEGIN 0
#define END N
#define LEFT(i)  ((i)>0?(i)-1:N-1)
#define RIGHT(i) ((i)<N-1?(i)+1:0)
#else
#define BEGIN 1
#define END (N-1)
#define LEFT(i)  ((i)-1)
#define RIGHT(i) ((i)+1)
#endif

RtoR::KGEnergy::KGEnergy(Core::Simulation::VoidBuilder &builder, RtoR::Function::Ptr potentialFunc)
: builder(builder)
, _oEnergyDensity (builder.NewFunctionArbitrary<RtoR::DiscreteFunction>())
, _oKineticDensity (builder.NewFunctionArbitrary<RtoR::DiscreteFunction>())
, _oGradientDensity (builder.NewFunctionArbitrary<RtoR::DiscreteFunction>())
, _oPotentialDensity (builder.NewFunctionArbitrary<RtoR::DiscreteFunction>())
, V_ptr(potentialFunc)
{

}

auto RtoR::KGEnergy::computeDensities(const RtoR::EquationState &field) -> const RtoR::DiscreteFunction &{
    auto &phi = field.getPhi(), &ddtPhi = field.getDPhiDt();
    auto &phiSpace = phi.getSpace(),
            &ddtPhiSpace = ddtPhi.getSpace();

    RealArray &e = _oEnergyDensity->getSpace().getHostData();
    RealArray &k = _oKineticDensity->getSpace().getHostData();
    RealArray &g = _oGradientDensity->getSpace().getHostData();
    RealArray &v = _oPotentialDensity->getSpace().getHostData();

    fix &V = *V_ptr;

    const floatt h = phiSpace.geth();
    const floatt inv2h = .5/h;
    auto &X = phiSpace.getHostData();
    auto &dXdt = ddtPhiSpace.getHostData();
    const int N = X.size();
    {
        for (int i = BEGIN; i < END; i++) {
            const Real xl = X[LEFT(i)],
                       xcAbs = V(X[i]),
                       xr = X[RIGHT(i)],
                       dxdt = dXdt[i];

            const Real dxdx = inv2h*(xr-xl);

            k[i] = .5*dxdt*dxdt;
            g[i] = .5*dxdx*dxdx;
            v[i] = xcAbs;

            e[i] = k[i] + g[i] + v[i];
        }
    }

    return *_oEnergyDensity;
}

auto RtoR::KGEnergy::integrateEnergy() -> Real
{
    RealArray &E_v = _oEnergyDensity->getSpace().getHostData();
    Real E=0;

    for(const auto &e : E_v)
        E += e;

    Real dx = builder.getNumericParams().geth();
    return E*dx;
}



auto RtoR::KGEnergy::integrateEnergy(Real xmin, Real xmax) -> Real {
    auto &func = *_oEnergyDensity;

    RealArray &E_v = _oEnergyDensity->getSpace().getHostData();
    Real dx = builder.getNumericParams().geth();

    PosInt iMin = func.mapPosToInt(xmin), iMax = func.mapPosToInt(xmax);

    Real E=0;
    for(PosInt i=iMin; i<iMax; ++i)
        E += E_v[i];

    return E*dx;

    return 0;
}

Real RtoR::KGEnergy::integrateKinetic() {
    RealArray &K_v = _oKineticDensity->getSpace().getHostData();
    Real K=0;

    for(const auto &k : K_v)
        K += k;

    Real dx = builder.getNumericParams().geth();
    return K*dx;
}

Real RtoR::KGEnergy::integrateGradient() {
    RealArray &Grad_v = _oGradientDensity->getSpace().getHostData();
    Real Grad=0;

    for(const auto &grad : Grad_v)
        Grad += grad;

    Real dx = builder.getNumericParams().geth();
    return Grad*dx;
}

Real RtoR::KGEnergy::integratePotential() {
    RealArray &Pot_v = _oPotentialDensity->getSpace().getHostData();
    Real Pot=0;

    for(const auto &pot : Pot_v)
        Pot += pot;

    Real dx = builder.getNumericParams().geth();
    return Pot*dx;
}
