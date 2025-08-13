//
// Created by joao on 24/12/24.
//

#include "RtoR-Hamiltonian-MetropolisHastings-Recipe.h"

#include "Core/SlabCore.h"
#include "Utils/RandUtils.h"

#include "Math/Numerics/Metropolis/MontecarloStepper.h"
#include "Math/Numerics/ODE/Output/Sockets/OutputConsoleMonitor.h"
#include "Math/Thermal/ThermoUtils.h"
#include "Math/Numerics/Metropolis/RtoR/RtoR-Metropolis-Config.h"

namespace Slab::Models::KGRtoR::Metropolis {

    RtoRHamiltonianMetropolisHastingsRecipe::RtoRHamiltonianMetropolisHastingsRecipe(UInt max_steps)
    : Base::NumericalRecipe(New<MetropolisRtoRConfig>(max_steps), "Metropolis-Hastings ℝ↦ℝ", "", DONT_REGISTER) {
        Core::RegisterCLInterface(Interface);
    }

    auto RtoRHamiltonianMetropolisHastingsRecipe::getField() -> FieldPair {
        if(field_data.ϕ == nullptr){
            assert(field_data.π == nullptr);

            fix x_min=-.5;
            fix x_max= .5;
            fix N=10000;

            field_data.ϕ = New<RtoR::NumericFunction_CPU>(N, x_min, x_max);
            field_data.π = New<RtoR::NumericFunction_CPU>(N, x_min, x_max);
        }

        return field_data;
    }

    auto RtoRHamiltonianMetropolisHastingsRecipe::BuildOutputSockets() -> Vector<TPointer<Socket>> {
        fix total_steps = getNumericConfig()->getn();

        auto console_monitor = New<OutputConsoleMonitor>(total_steps);
        console_monitor->setnSteps((int)1000);

        return {console_monitor};
    }

    auto RtoRHamiltonianMetropolisHastingsRecipe::buildStepper() -> TPointer<Stepper> {
        RtoRMetropolisSetup setup;

        Temperature T=1E-2;
        constexpr auto δϕₘₐₓ = 1e-0;
        constexpr auto δπₘₐₓ = 1e-2;

        auto field = getField();

        setup.should_accept = [T](DevFloat ΔE) {
            return RandUtils::RandomUniformReal01() < Min(1.0, exp(-ΔE / T));
        };;

        using RandomSite = RtoRMetropolisSetup::RandomSite;
        using NewValue = RtoRMetropolisSetup::NewValue;

        setup.draw_value = [field](RandomSite site){
            fix ϕ_old = field.ϕ->getSpace().getHostData()[site];
            fix π_old = field.π->getSpace().getHostData()[site];

            return NewValue{RandUtils::RandomUniformReal(ϕ_old-δϕₘₐₓ, ϕ_old+δϕₘₐₓ),
                            RandUtils::RandomUniformReal(π_old-δπₘₐₓ, π_old+δπₘₐₓ)};
        };

        setup.ΔS = [field](RandomSite n, NewValue new_val) {
            fix N = field.ϕ->N;
            auto φₖ = [field, N](int n) {
                fix i = n<0 ? N+n : n>N-1 ? n-N : n;
                return field.ϕ->getSpace().getHostData()[i];
            };

            fix φ = φₖ(n);
            fix ϕ = new_val.first;

            fix ϖ = field.π->getSpace().getHostData()[n];
            fix π = new_val.second;

            fix Δx = field.ϕ->getSpace().getMetaData().geth(0);

            constexpr auto sign = Slab::Math::SIGN<DevFloat>;

            fix δK = .5 * (sqr(ϖ) - sqr(π));
            fix δV = fabs(φ) - fabs(ϕ);
            fix δW = (.25/(Δx*Δx)) * ((ϕ-φ)*(φₖ(n-2)+φₖ(n+2)) + sqr(φ) - sqr(ϕ));

            return Δx * (δK + δW + δV);
        };

        setup.sample_locations = [field](){
            constexpr auto border_size = 0;

            fix N = field.ϕ->N;

            Vector<RandomSite> samples(N);

            for(auto &site : samples)
                site = border_size + RandUtils::RandomUniformUInt() % (N - 2 * border_size);

            return samples;
        };

        setup.modify = [field](RandomSite n, NewValue value){
            field.ϕ->getSpace().getHostData()[n] = value.first;
            field.ϕ->getSpace().getHostData()[n] = value.second;
        };

        auto metropolis = New<RtoRMetropolis>(setup);

        return New<MontecarloStepper<RandomSite, NewValue>>(metropolis);
    }

}