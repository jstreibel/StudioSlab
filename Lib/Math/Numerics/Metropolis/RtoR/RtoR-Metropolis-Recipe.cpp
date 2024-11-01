//
// Created by joao on 10/28/24.
//

#include "RtoR-Metropolis-Recipe.h"

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

    class MetropolisRtoRConfig : public NumericConfig {
        UInt max_steps;

    public:
        explicit MetropolisRtoRConfig(UInt max_steps)
        : NumericConfig(DONT_REGISTER), max_steps(max_steps) {
            // registerToManager();
        }

        auto getn() const -> UInt override{ return max_steps; };

        auto to_string() const -> Str override{ return {}; };
    };

    auto RtoRMetropolisRecipe::getField() -> Pointer<RtoR::NumericFunction_CPU> {
        if(field_data == nullptr){
            fix t_min=0.;
            fix t=2.;
            fix N=100;

            field_data = New<RtoR::NumericFunction_CPU>(N, t_min, t_min+t);
        }

        return field_data;
    }

    RtoRMetropolisRecipe::RtoRMetropolisRecipe(UInt max_steps)
    : Base::NumericalRecipe(New<MetropolisRtoRConfig>(max_steps), "Metropolis R2->R", "", DONT_REGISTER) {
        // Core::RegisterCLInterface(interface);
    }

    Vector<Pointer<Socket>> RtoRMetropolisRecipe::buildOutputSockets() {
        fix total_steps = getNumericConfig()->getn();

        auto console_monitor = New<OutputConsoleMonitor>(total_steps);
        console_monitor->setnSteps((int)1000);

        return {console_monitor};
    }

    Pointer<Stepper> RtoRMetropolisRecipe::buildStepper() {
        RtoRMetropolisSetup setup;

        Temperature T=1E-2;
        constexpr auto δϕₘₐₓ = 1e-2;

        auto field = getField();

        auto acceptance_thermal = [T](Real ΔE) {
            return RandUtils::RandomUniformReal01() < Min(1.0, exp(-ΔE / T));
        };

        auto acceptance_action = [](Real Δ_δSδϕ) {
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

            auto compute_δSδq = [affected_sites, Δt2, q](Real &δSδϕ) {
                constexpr auto sign = Slab::Math::SIGN<Real>;
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

        setup.Δ_δSδϕ = Δ_δSδϕ;

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