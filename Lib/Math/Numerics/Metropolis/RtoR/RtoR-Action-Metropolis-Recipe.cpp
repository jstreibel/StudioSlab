//
// Created by joao on 10/28/24.
//

#include "RtoR-Action-Metropolis-Recipe.h"

#include "Core/SlabCore.h"
#include "Utils/RandUtils.h"

#include "Math/Numerics/Metropolis/MontecarloStepper.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"
#include "Math/Thermal/ThermoUtils.h"
#include "RtoR-Metropolis-Config.h"


namespace Slab::Math {

    // inline DevFloat sqr(const DevFloat &v){ return v*v; }

    auto RtoRActionMetropolisRecipe::getField() -> TPointer<RtoR::NumericFunction_CPU> {
        if(field_data == nullptr){
            fix t_min=0.;
            fix t=2.;
            fix N=100;

            field_data = New<RtoR::NumericFunction_CPU>(N, t_min, t_min+t);
        }

        return field_data;
    }

    RtoRActionMetropolisRecipe::RtoRActionMetropolisRecipe(UInt max_steps)
    : Base::FNumericalRecipe(New<MetropolisRtoRConfig>(max_steps), "Metropolis R2->R", "", DONT_REGISTER) {
        // Core::RegisterCLInterface(interface);
    }

    Vector<TPointer<FOutputChannel>> RtoRActionMetropolisRecipe::BuildOutputSockets() {
        fix total_steps = GetNumericConfig()->Get_n();

        auto console_monitor = New<OutputConsoleMonitor>(total_steps);
        console_monitor->Set_nSteps((int)1000);

        return {console_monitor};
    }

    TPointer<FStepper> RtoRActionMetropolisRecipe::BuildStepper() {
        RtoRMetropolisSetup setup;

        Temperature T=1E-2;
        constexpr auto δϕₘₐₓ = 1e-2;

        auto field = getField();

        auto acceptance_thermal = [T](DevFloat ΔE) {
            return RandUtils::RandomUniformReal01() < Min(1.0, exp(-ΔE / T));
        };

        auto acceptance_action = [](DevFloat Δ_δSδϕ) {
            return Δ_δSδϕ<0;
        };


        if(T!=0) setup.should_accept = acceptance_thermal;
        else     setup.should_accept = acceptance_action;

        using RandomSite = RtoRMetropolisSetup::RandomSite;
        using NewValue = RtoRMetropolisSetup::NewValue;

        setup.draw_value = [field](RandomSite site){
            fix old_val = field->getSpace().getHostData()[site];
            return RandUtils::RandomUniformReal(old_val-δϕₘₐₓ, old_val+δϕₘₐₓ);
        };

        auto Δ_δSδϕ          = [field](RandomSite n, NewValue new_val) {
            auto q = [field](int n) { return field->getSpace().getHostData()[n]; };

            fix qₒₗ = q(n);
            fix Q = new_val;

            fix Δt = field->getSpace().getMetaData().geth(0);
            fix Δt2 = Δt*Δt;

            fix N = field->N;

            Vector<RandomSite> affected_sites;
            if(n==1) affected_sites = {n, n+1};
            else if(n==N-2) affected_sites = {n, n-1};
            else affected_sites = {n+1, n, n-1};

            auto compute_δSδq = [affected_sites, Δt2, q](DevFloat &δSδϕ) {
                constexpr auto sign = Slab::Math::SIGN<DevFloat>;
                for(auto nₗ : affected_sites) {
                    δSδϕ += sqr((q(nₗ + 1) - 2. * q(nₗ) + q(nₗ - 1)) / Δt2
                              + sign(q(nₗ)));
                }
            };

            auto δSδq_old = .0;
            auto δSδq_new = .0;

            compute_δSδq(δSδq_old);
            field->getSpace().getHostData()[n] = Q;
            compute_δSδq(δSδq_new);
            field->getSpace().getHostData()[n] = qₒₗ;

            return (δSδq_new-δSδq_old)*Δt;
        };

        setup.ΔS = Δ_δSδϕ;

        setup.sample_locations = [field](){
            constexpr auto border_size = 1;

            Vector<RandomSite> samples(field->N);

            for(auto &site : samples)
                site = border_size + RandUtils::RandomUniformUInt() % (field->N - 2 * border_size);

            return samples;
        };

        setup.modify = [field](RandomSite n, NewValue value){
            field->getSpace().getHostData()[n] = value;
        };

        auto metropolis = New<RtoRMetropolis>(setup);

        return New<MontecarloStepper<RandomSite, NewValue>>(metropolis);
    }

} // Slab::Math