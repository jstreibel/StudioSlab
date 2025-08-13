//
// Created by joao on 17/04/2021.
//

#include "KG-RtoREnergyCalculator.h"
#include "Math/Function/RtoR/Model/Operators/DerivativesCPU.h"
#include "KG-RtoR-Recipe.h"


// #define USE_PERIODIC_BC

namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    bool check_consistency(const RtoR::NumericFunction& a, const RtoR::NumericFunction& b) {
        if(a.getSpace().getMetaData().getN(0) != b.getSpace().getMetaData().getN(0)){
            Log::Error() << "a.N!=b.N <=> " << a.getSpace().getMetaData().getN(0) << "!=" << b.getSpace().getMetaData().getN(0) << Log::Flush;
            return false;
        }

        if(!Common::AreEqual(a.xMin, b.xMin, 1.e-6)) {
            Log::Error() << "a.x_min!=b.x_min <=> " << a.xMin << "!=" << b.xMin << Log::Flush;

            return false;
        }if(!Common::AreEqual(a.xMax, b.xMax, a.xMax*1.e-6)) {
            Log::Error() << "a.x_max!=b.x_max <=> " << a.xMax << "!=" << b.xMax << Log::Flush;
            return false;
        }


        return true;
    }

    KGEnergy::KGEnergy(RtoR::Function_ptr potentialFunc) : V_ptr(potentialFunc) {    }

    auto KGEnergy::computeEnergies(const RtoR::NumericFunction& phi, const RtoR::NumericFunction& ddtPhi) -> const RtoR::NumericFunction & {
        if(!check_consistency(phi, ddtPhi)) {
            Log::Error() << Str("Inconsistency at ") + __PRETTY_FUNCTION__ + ":" + ToStr(__LINE__) << Log::Flush;
            return *_oEnergyDensity;
        }

        if(_oEnergyDensity == nullptr || !check_consistency(*_oEnergyDensity, phi)) {
            _oEnergyDensity   = DynamicPointerCast<RtoR::NumericFunction>(phi.Clone());
            _oKineticDensity  = DynamicPointerCast<RtoR::NumericFunction>(phi.Clone());
            _oGradientDensity = DynamicPointerCast<RtoR::NumericFunction>(phi.Clone());
            _oPotentialDensity= DynamicPointerCast<RtoR::NumericFunction>(phi.Clone());
        }

        auto &phiSpace = phi   .getSpace(),
          &ddtPhiSpace = ddtPhi.getSpace();

        RealArray &e = _oEnergyDensity   ->getSpace().getHostData();
        RealArray &k = _oKineticDensity  ->getSpace().getHostData();
        RealArray &g = _oGradientDensity ->getSpace().getHostData();
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
                const DevFloat xcAbs = V_func(Φ[i]),
                        dϕdt = dΦdt[i];
                const DevFloat dϕdx = dΦdx[i];

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

        DevFloat dx = phi.getSpace().getMetaData().geth(0);
        U *= dx;
        K *= dx;
        W *= dx;
        V *= dx;

        return *_oEnergyDensity;
    }

    auto KGEnergy::GetTotalEnergy() const -> DevFloat { return U; }


    auto KGEnergy::integrateEnergy(DevFloat xmin, DevFloat xmax) -> DevFloat {
        auto &func = *_oEnergyDensity;

        RealArray &E_v = _oEnergyDensity->getSpace().getHostData();
        DevFloat dx = _oEnergyDensity->getSpace().getMetaData().geth(0);

        UInt iMin = func.mapPosToInt(xmin), iMax = func.mapPosToInt(xmax);

        DevFloat E = 0;
        for (UInt i = iMin; i < iMax; ++i)
            E += E_v[i];

        return E * dx;

        return 0;
    }

    DevFloat KGEnergy::GetTotalKineticEnergy() const { return K; }

    DevFloat KGEnergy::GetTotalGradientEnergy() const { return W; }

    DevFloat KGEnergy::GetTotalPotentialEnergy() const { return V; }


}