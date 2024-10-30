//
// Created by joao on 10/28/24.
//

#include "R2toR-Metropolis-Recipe.h"
#include "R2toR-Metropolis.h"

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

    auto R2toRMetropolisRecipe::getField() -> Pointer<R2toR::NumericFunction_CPU> {
        if(field_data == nullptr){
            fix N=100, M=100;
            fix x_min=-5.0, y_min=0.0;
            fix Lx=10.0, Ly=10.0;

            field_data = New<R2toR::NumericFunction_CPU>(100, 100, x_min, y_min, Lx/Real(N-1), Ly/Real(M-1));

            DataManager::RegisterData("Stochastic field", field_data);
        }

        return field_data;
    }

    R2toRMetropolisRecipe::R2toRMetropolisRecipe(UInt max_steps)
    : Base::NumericalRecipe(New<MetropolisR2toRConfig>(max_steps), "Metropolis R2->R", "", DONT_REGISTER) {
        // Core::RegisterCLInterface(interface);
    }

    Vector<Pointer<Socket>> R2toRMetropolisRecipe::buildOutputSockets() {
        fix total_steps = getNumericConfig()->getn();

        auto console_monitor = New<OutputConsoleMonitor>(total_steps);
        console_monitor->setnSteps((int)total_steps/10);

        return {console_monitor};
    }

    Pointer<Stepper> R2toRMetropolisRecipe::buildStepper() {
        R2toRMetropolisSetup setup;

        Temperature T=0.2;
        constexpr auto dϕ = 0.1;

        auto field = getField();

        setup.draw_value = [field](RandomSite site){
            fix old_val = field->At(site.i, site.j);

            return RandUtils::RandomUniformReal(old_val-dϕ, old_val+dϕ);
        };

        setup.ΔS = [field](RandomSite site, NewValue new_val){
            fix n = site.i;
            fix m = site.j;

            fix N = field->At(n, m+1);
            fix S = field->At(n, m-1);
            fix W = field->At(n-1, m);
            fix E = field->At(n+1, m);
            fix C = field->At(n, m);
            fix φ = new_val;

            fix kinetic_old = .5 * (sqr(N-C) + sqr(C-S));
            fix grad_old    = .5 * (sqr(E-C) + sqr(C-W));
            fix pot_old     = Abs(C);

            fix kinetic_new = .5 * (sqr(N-φ) + sqr(φ-S));
            fix grad_new    = .5 * (sqr(E-φ) + sqr(φ-W));
            fix pot_new     = Abs(φ);

            fix &meta_data =field->getSpace().getMetaData();
            fix δS = meta_data.geth(0)*meta_data.geth(1);

            return δS*((kinetic_new-grad_new-pot_new) - (kinetic_old-grad_old-pot_old));
        };

        setup.sample = [field](){
            fix i = RandUtils::RandomUniformUInt()%field->getN();
            fix j = RandUtils::RandomUniformUInt()%field->getM();
            return RandomSite{i, j};
        };

        setup.modify = [field](RandomSite site, NewValue value){
            field->At(site.i, site.j) = value;
        };

        setup.should_accept = [T](Real ΔS){
            return ΔS < 0;
        };

        auto metropolis = New<R2toRMetropolis>(field, setup, T);

        return New<MontecarloStepper>(metropolis);
    }

} // Slab::Math