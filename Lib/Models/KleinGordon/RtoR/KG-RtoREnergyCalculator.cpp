//
// Created by joao on 17/04/2021.
//

#include "KG-RtoREnergyCalculator.h"
#include "Math/Function/RtoR/Model/Operators/DerivativesCPU.h"
#include "KG-RtoRBuilder.h"

// #define USE_PERIODIC_BC

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    KGEnergy::KGEnergy(KGRtoRBuilder &builder, RtoR::Function_ptr potentialFunc)
            : builder(builder), _oEnergyDensity(builder.newFunctionArbitrary())
            , _oKineticDensity(   builder.newFunctionArbitrary() )
            , _oGradientDensity(  builder.newFunctionArbitrary() )
            , _oPotentialDensity( builder.newFunctionArbitrary() )
            , V_ptr(potentialFunc) {

    }

    auto KGEnergy::computeEnergies(const EquationState &field) -> const RtoR::NumericFunction & {
        auto &phi = static_cast<RtoR::NumericFunction&>(field.getPhi()),
          &ddtPhi = static_cast<RtoR::NumericFunction&>(field.getDPhiDt());

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

    auto KGEnergy::getTotalEnergy() const -> Real { return U; }


    auto KGEnergy::integrateEnergy(Real xmin, Real xmax) -> Real {
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

    Real KGEnergy::getTotalKineticEnergy() const { return K; }

    Real KGEnergy::getTotalGradientEnergy() const { return W; }

    Real KGEnergy::getTotalPotentialEnergy() const { return V; }


}