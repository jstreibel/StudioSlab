//
// Created by joao on 10/28/24.
//

#include "R2toR-Metropolis-Recipe.h"

#include "Core/SlabCore.h"
#include "Utils/RandUtils.h"

#include "Math/Numerics/Metropolis/MontecarloStepper.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"
#include "Math/Thermal/ThermoUtils.h"


namespace Slab::Math {

    // inline DevFloat sqr(const DevFloat &v){ return v*v; }

    // Don't touch:
    #define DONT_REGISTER false
    #define DO_REGISTER true

    R2toRMetropolisRecipe::R2toRMetropolisRecipe(UInt max_steps)
    : Base::FNumericalRecipe(New<MetropolisConfig>(max_steps), "Metropolis R2->R", "", DONT_REGISTER) {
        // Core::RegisterCLInterface(interface);
    }

    Vector<TPointer<Socket>> R2toRMetropolisRecipe::BuildOutputSockets() {
        fix total_steps = GetNumericConfig()->getn();

        auto console_monitor = New<OutputConsoleMonitor>(total_steps);
        console_monitor->setnSteps((int)1000);

        return {console_monitor};
    }

    auto R2toRMetropolisRecipe::getField() -> TPointer<R2toR::NumericFunction_CPU> {
        if(field_data == nullptr){
            fix x_min=-.6, y_min=0.;
            fix L=-2*x_min, t=2.; ///(Real(N)/Real(M));
            fix N=6, M=6;

            field_data = DataAlloc<R2toR::NumericFunction_CPU>("Stochastic field",
                                                               N, M,
                                                               x_min, y_min,
                                                               L/DevFloat(N), t/DevFloat(M));
        }

        return field_data;
    }

    TPointer<Stepper> R2toRMetropolisRecipe::buildStepper() {
        R2toRMetropolisSetup setup;

        Temperature T=0.1;
        constexpr auto δϕₘₐₓ = 2e-4;

        auto field = getField();

        auto acceptance_thermal = [T](DevFloat ΔE) {
            auto rand = RandUtils::RandomUniformReal01;
            return rand() < Min(1.0, exp(-ΔE / T));
        };

        auto acceptance_action = [](DevFloat Δ_δSδϕ) {
            return Δ_δSδϕ<0;
        };


        if(T!=0) setup.should_accept = acceptance_thermal;
        else     setup.should_accept = acceptance_action;

        setup.draw_value = [field](RandomSite site){
            fix old_val = field->At(site.i, site.j);

            return RandUtils::RandomUniformReal(old_val-δϕₘₐₓ, old_val+δϕₘₐₓ);
        };

        setup.ΔS = [field](RandomSite site, NewValue new_val) {
            fix i = site.i;
            fix n = site.j;

            auto ϕ = [field](int i, int n) {
                return field->At(i, n);
            };

            fix ϕₒₗ = ϕ(i,n);
            fix φ = new_val;

            fix &h2 = field->getSpace().getMetaData().geth();
            fix Δx = h2[0];
            fix Δt = h2[1];

            fix ΔxΔt = Δx*Δt;
            fix Δx2 = Δx*Δx;
            fix Δt2 = Δt*Δt;

            auto N = field->getN();
            auto M = field->getM();

            Vector<RandomSite> affected_sites = {{i, n}};

            if(0) {
                affected_sites.push_back({i + 1, n});
                affected_sites.push_back({i - 1, n});
                affected_sites.push_back({i, n - 1});
                affected_sites.push_back({i, n + 1});
            } else {
                if      (i == 1    ) affected_sites.push_back({i + 1, n});
                else if (i == N - 2) affected_sites.push_back({i - 1, n});
                else {
                    affected_sites.push_back({i + 1, n});
                    affected_sites.push_back({i - 1, n});
                }
                if (n == 1) affected_sites.push_back({i, n + 1});
                else if (n == M - 2) affected_sites.push_back({i, n - 1});
                else {
                    affected_sites.push_back({i, n - 1});
                    affected_sites.push_back({i, n + 1});
                }
            }
            constexpr auto sign = Slab::Math::SIGN<DevFloat>;
            auto compute_δSδϕ2 = [affected_sites, Δx2, Δt2, ϕ]() {
                DevFloat δSδϕ2 = .0;

                for(auto s : affected_sites) {
                    fix iₗ = s.i;
                    fix nₗ = s.j;

                    δSδϕ2 += sqr((ϕ(iₗ  , nₗ+1) - 2.*ϕ(iₗ, nₗ) + ϕ(iₗ  , nₗ-1)) / Δt2
                                 - (ϕ(iₗ+1, nₗ  ) - 2.*ϕ(iₗ, nₗ) + ϕ(iₗ-1, nₗ  )) / Δx2
                                 + sign(ϕ(iₗ, nₗ)));
                }

                return δSδϕ2;
            };

            auto δSδϕ2_old = compute_δSδϕ2();
            field->At(i,n) = φ;
            auto δSδϕ2_new = compute_δSδϕ2();
            field->At(i,n) = ϕₒₗ;

            return δSδϕ2_new-δSδϕ2_old;
        };

        setup.sample_locations = [field](){
            constexpr auto h_border_size = 1;
            constexpr auto v_border_size = 1; // zero means periodic

            static_assert(h_border_size<=1);
            static_assert(v_border_size<=1);

            Vector<RandomSite> sites(field->getN()*field->getM());

            for(OUT site : sites)
                site = {h_border_size + RandUtils::RandomUniformUInt() % (field->getN() - 2*h_border_size),
                        v_border_size + RandUtils::RandomUniformUInt() % (field->getM() - 2*v_border_size)};

            return sites;
        };

        setup.modify = [field](RandomSite site, NewValue value){ field->At(site.i, site.j) = value; };

        auto metropolis = New<R2toRMetropolis>(setup);

        return New<MontecarloStepper<RandomSite, NewValue>>(metropolis);
    }

} // Slab::Math