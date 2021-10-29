//
// Created by joao on 17/04/2021.
//

#include "EnergyCalculator.h"

auto RtoR::EnergyCalculator::computeEnergyDensity(const RtoR::FieldState &field) -> const RtoR::FunctionArbitrary &{
    auto &phi = field.getPhi(), &ddtPhi = field.getDPhiDt();
    auto &phiSpace = phi.getSpace(),
            &ddtPhiSpace = ddtPhi.getSpace();
    VecFloat &E = _oEnergyDensityFunc->getSpace().getX();

    const float invh = 1./phiSpace.geth();
    const float c = .25*invh*invh;
    auto &X = phiSpace.getX();
    auto &dXdt = ddtPhiSpace.getX();
    const int N = X.size();
    {
        E[0] = .0;
        E[N - 1] = .0;
        for (int i = 1; i < N-1; i++) {
            const Real xl = X[i-1], xcAbs = abs(X[i]), xr = X[i+1], dxdt = dXdt[i];

            const Real dxdx = 0.5*invh*(xr-xl);
            const Real val = 0.5*(dxdx*dxdx + dxdt*dxdt) + xcAbs;
            E[i] = .5*(dxdx*dxdx + dxdt*dxdt) + xcAbs;
        }
    }

    return *_oEnergyDensityFunc;
}

auto RtoR::EnergyCalculator::integrate() -> Real
{
    VecFloat &E_v = _oEnergyDensityFunc->getSpace().getX();
    Real E=0;
    for(const auto &e : E_v)
        E += e;

    Real dx = Allocator::getInstance().getNumericParams().geth();
    return E*dx;
}

auto RtoR::EnergyCalculator::integrate(Real xmin, Real xmax) -> Real {
    auto &func = *_oEnergyDensityFunc;

    VecFloat &E_v = _oEnergyDensityFunc->getSpace().getX();
    Real dx = Allocator::getInstance().getNumericParams().geth();

    PosInt iMin = func.mapPosToInt(xmin), iMax = func.mapPosToInt(xmax);

    Real E=0;
    for(PosInt i=iMin; i<iMax; ++i)
        E += E_v[i];

    return E*dx;

    return 0;
}
