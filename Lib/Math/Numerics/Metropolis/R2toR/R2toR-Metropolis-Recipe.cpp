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

    // inline Real sqr(const Real &v){ return v*v; }

    // Don't touch:
    #define DONT_REGISTER false
    #define DO_REGISTER true

    R2toRMetropolisRecipe::R2toRMetropolisRecipe(UInt max_steps)
    : Base::NumericalRecipe(New<MetropolisConfig>(max_steps), "Metropolis R2->R", "", DONT_REGISTER) {
        // Core::RegisterCLInterface(interface);
    }

    Vector<Pointer<Socket>> R2toRMetropolisRecipe::buildOutputSockets() {
        fix total_steps = getNumericConfig()->getn();

        auto console_monitor = New<OutputConsoleMonitor>(total_steps);
        console_monitor->setnSteps((int)100);

        return {console_monitor};
    }

    auto R2toRMetropolisRecipe::getField() -> Pointer<R2toR::NumericFunction_CPU> {
        if(field_data == nullptr){
            fix x_min=-1.2, y_min=0.;
            fix Lx=-2*x_min, Ly=4.; ///(Real(N)/Real(M));
            fix N=10, M=N*int(Real(Ly)/Lx);

            field_data = DataAlloc<R2toR::NumericFunction_CPU>("Stochastic field",
                                                               N, M,
                                                               x_min, y_min,
                                                               Lx/Real(N), Ly/Real(M));
        }

        return field_data;
    }

    Pointer<Stepper> R2toRMetropolisRecipe::buildStepper() {
        R2toRMetropolisSetup setup;

        Temperature T=0;
        constexpr auto δϕₘₐₓ = 1.25e-2;

        auto field = getField();

        auto acceptance_thermal = [T](Real ΔE) {
            return RandUtils::RandomUniformReal01() < Min(1.0, exp(-ΔE / T));
        };

        auto acceptance_action = [](Real Δ_δSδϕ) {
            return Δ_δSδϕ<0;
        };


        if(T!=0) setup.should_accept = acceptance_thermal;
        else     setup.should_accept = acceptance_action;

        setup.draw_value = [field](RandomSite site){
            fix old_val = field->At(site.i, site.j);

            return RandUtils::RandomUniformReal(old_val-δϕₘₐₓ, old_val+δϕₘₐₓ);
        };

        auto Δ_δSδϕ          = [field](RandomSite site, NewValue new_val) {
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

            if(i==1)        affected_sites.push_back({i + 1, n});
            else if(i==N-2) affected_sites.push_back({i - 1, n});
            else {          affected_sites.push_back({i + 1, n});
                            affected_sites.push_back({i - 1, n});
            }

            if(n==1)        affected_sites.push_back({i, n + 1});
            else if(n==M-2) affected_sites.push_back({i, n - 1});
            else {          affected_sites.push_back({i, n - 1});
                            affected_sites.push_back({i, n + 1});
            }

            auto compute_δSδϕ = [affected_sites, Δx2, Δt2, ϕ](Real &δSδϕ) {
                constexpr auto sign = Slab::Math::SIGN<Real>;

                for(auto s : affected_sites) {
                    fix iₗ = s.i;
                    fix nₗ = s.j;

                    δSδϕ += sqr((ϕ(iₗ  , nₗ + 1) - 2. * ϕ(iₗ, nₗ) + ϕ(iₗ  , nₗ - 1)) / Δt2
                              - (ϕ(iₗ + 1, nₗ  ) - 2. * ϕ(iₗ, nₗ) + ϕ(iₗ - 1, nₗ  )) / Δx2
                              + sign(ϕ(iₗ, nₗ)));
                }
            };

            auto δSδϕ_old = .0;
            compute_δSδϕ(δSδϕ_old);

            field->At(i,n) = φ;
            auto δSδϕ_new = .0;
            compute_δSδϕ(δSδϕ_new);
            field->At(i,n) = ϕₒₗ;

            return sqrt(δSδϕ_new-δSδϕ_old)*ΔxΔt;
        };

        setup.Δ_δSδϕ = Δ_δSδϕ;

        fix h_border_size = 0; //field->getN()/4;
        fix v_border_size = 1; //field->getM()/4;;

        setup.sample_locations = [field,h_border_size,v_border_size](){

            Vector<RandomSite> sites(field->getN()*field->getM());

            for(OUT site : sites)
                site = {h_border_size + RandUtils::RandomUniformUInt() % (field->getN() - 2 * h_border_size),
                        v_border_size + RandUtils::RandomUniformUInt() % (field->getM() - 2 * v_border_size)};

            return sites;
        };

        setup.modify = [field](RandomSite site, NewValue value){
            field->At(site.i, site.j) = value;
        };

        auto metropolis = New<R2toRMetropolis>(setup);

        return New<MontecarloStepper<RandomSite, NewValue>>(metropolis);
    }

} // Slab::Math