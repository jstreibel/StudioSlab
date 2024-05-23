//
// Created by joao on 17/04/2021.
//

#include "KG-RtoREnergyCalculator.h"
#include "Math/Function/RtoR/Model/Derivatives/DerivativesCPU.h"

#include "Math/Numerics/VoidBuilder.h"
#include "KG-RtoREquationState.h"

// #define USE_PERIODIC_BC

namespace Slab::Math {

    RtoR::KGEnergy::KGEnergy(VoidBuilder &builder, RtoR::Function_ptr potentialFunc)
            : builder(builder), _oEnergyDensity(builder.NewFunctionArbitrary<RtoR::DiscreteFunction>()),
              _oKineticDensity(builder.NewFunctionArbitrary<RtoR::DiscreteFunction>()),
              _oGradientDensity(builder.NewFunctionArbitrary<RtoR::DiscreteFunction>()),
              _oPotentialDensity(builder.NewFunctionArbitrary<RtoR::DiscreteFunction>()), V_ptr(potentialFunc) {

    }

    auto RtoR::KGEnergy::computeEnergies(const RtoR::EquationState &field) -> const RtoR::DiscreteFunction & {
        auto &phi = field.getPhi(), &ddtPhi = field.getDPhiDt();
        auto &phiSpace = phi.getSpace(),
                &ddtPhiSpace = ddtPhi.getSpace();

        RealArray &e = _oEnergyDensity->getSpace().getHostData();
        RealArray &k = _oKineticDensity->getSpace().getHostData();
        RealArray &g = _oGradientDensity->getSpace().getHostData();
        RealArray &v = _oPotentialDensity->getSpace().getHostData();

        fix &V_func = *V_ptr;

        auto &Φ = phiSpace.getHostData();
        auto &dΦdt = ddtPhiSpace.getHostData();
        const int N = Φ.size();

        U = .0;
        K = .0;
        W = .0;
        V = .0;

        RtoR::DerivativeCPU derivatves(phi);
        RealArray_O dΦdx(phi.N);
        {
            derivatves.dfdx_v(dΦdx);

            for (int i = 0; i < N; i++) {
                const Real xcAbs = V_func(Φ[i]),
                        dϕdt = dΦdt[i];
                const Real dϕdx = dΦdx[i];

                k[i] = .5 * dϕdt * dϕdt;
                g[i] = .5 * dϕdx * dϕdx;
                v[i] = xcAbs;

                e[i] = k[i] + g[i] + v[i];

                U += e[i];
                K += k[i];
                W += g[i];
                V += v[i];
            }
        }

        Real dx = builder.getNumericParams().geth();
        U *= dx;
        K *= dx;
        W *= dx;
        V *= dx;

        return *_oEnergyDensity;
    }

    auto RtoR::KGEnergy::getTotalEnergy() const -> Real { return U; }


    auto RtoR::KGEnergy::integrateEnergy(Real xmin, Real xmax) -> Real {
        auto &func = *_oEnergyDensity;

        RealArray &E_v = _oEnergyDensity->getSpace().getHostData();
        Real dx = builder.getNumericParams().geth();

        UInt iMin = func.mapPosToInt(xmin), iMax = func.mapPosToInt(xmax);

        Real E = 0;
        for (UInt i = iMin; i < iMax; ++i)
            E += E_v[i];

        return E * dx;

        return 0;
    }

    Real RtoR::KGEnergy::getTotalKineticEnergy() const { return K; }

    Real RtoR::KGEnergy::getTotalGradientEnergy() const { return W; }

    Real RtoR::KGEnergy::getTotalPotentialEnergy() const { return V; }


}