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
            fix N=300, M=100;
            fix x_min=-2.0, y_min=0.0;
            fix Lx=4.0, Ly=4.0/(Real(N)/Real(M));

            field_data = New<R2toR::NumericFunction_CPU>(N, M,
                                                         x_min, y_min,
                                                         Lx/Real(N-1), Ly/Real(M-1));

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
        console_monitor->setnSteps((int)100);

        return {console_monitor};
    }

    Pointer<Stepper> R2toRMetropolisRecipe::buildStepper() {
        R2toRMetropolisSetup setup;

        Temperature T=0;
        constexpr auto dϕ = 5e-4;

        auto field = getField();

        auto acceptance_thermal = [T](Real ΔS) {
            return RandUtils::RandomUniformReal01() < Min(1.0, exp(-ΔS / T));
        };
        auto acceptance_nonthermal = [](Real ΔS) {
            return ΔS<0;
        };

        if(T==0) setup.should_accept = acceptance_nonthermal;
        else setup.should_accept = acceptance_thermal;

        setup.draw_value = [field](RandomSite site){
            fix old_val = field->At(site.i, site.j);

            return RandUtils::RandomUniformReal(old_val-dϕ, old_val+dϕ);
        };

        auto ΔS_fw_deriv     = [field](RandomSite site, NewValue new_val){
            fix n = site.i;
            fix m = site.j;

            fix N = field->At(n  , m+1);
            fix S = field->At(n  , m-1);
            fix W = field->At(n-1, m);
            fix E = field->At(n+1, m);
            fix C = field->At(n  , m);
            fix φ = new_val;

            fix &meta_data =field->getSpace().getMetaData();
            fix &hδt = meta_data.geth();
            fix h = hδt[0];
            fix δt = hδt[1];

            fix kinetic_old = (.5/sqr(δt)) * (sqr(N-C) + sqr(C-S));
            fix grad_old    = (.5/sqr(h))  * (sqr(E-C) + sqr(C-W));
            fix pot_old     = Abs(C);

            fix kinetic_new = (.5/sqr(δt)) * (sqr(N-φ) + sqr(φ-S));
            fix grad_new    = (.5/sqr(h))  * (sqr(E-φ) + sqr(φ-W));
            fix pot_new     = Abs(φ);

            fix δS = h*δt;

            return δS*((kinetic_new-grad_new-pot_new) - (kinetic_old-grad_old-pot_old));
        };
        auto ΔS_sym_deriv    = [field](RandomSite site, NewValue new_val){
            fix n = site.i;
            fix m = site.j;

            fix N = field->At(n  , m+2);
            fix S = field->At(n  , m-2);
            fix W = field->At(n-2, m);
            fix E = field->At(n+2, m);
            fix C = field->At(n  , m);
            fix φ = new_val;

            fix &meta_data =field->getSpace().getMetaData();
            fix &hδt = meta_data.geth();
            fix h = hδt[0];
            fix δt = hδt[1];

            fix kinetic_old = (.5/sqr(2*δt)) * (sqr(N-S) + sqr(C-S));
            fix grad_old    = (.5/sqr(2*h))  * (sqr(E-C) + sqr(C-W));
            fix pot_old     = Abs(C);

            fix kinetic_new = (.5/sqr(2*δt)) * (sqr(N-φ) + sqr(φ-S));
            fix grad_new    = (.5/sqr(2*h))  * (sqr(E-φ) + sqr(φ-W));
            fix pot_new     = Abs(φ);

            fix δS = h*δt;

            return δS*((kinetic_new-grad_new-pot_new) - (kinetic_old-grad_old-pot_old));
        };
        auto ΔS_sym_deriv_O2 = [field](RandomSite site, NewValue new_val){
            fix n = site.i;
            fix m = site.j;

            constexpr fix o=2;
            constexpr fix total = 2*(2*o) + 1;
            constexpr Real c[] = {1., -8., 0, 8., 1.};
            constexpr Real factor = 12.;

            Vector<Real> v_vals(total), h_vals(total);
            for(int i=0; i<total; ++i) {
                fix di = i - 2*o;
                v_vals[i] = field->At(n, m + di);
                h_vals[i] = field->At(n + di, m);

            }


            fix C = field->At(n  , m);
            fix φ = new_val;

            fix &meta_data =field->getSpace().getMetaData();
            fix &hδt = meta_data.geth();
            fix h = hδt[0];
            fix δt = hδt[1];

            /**
             * n_ ∈ [-2o, 2o]
             * n_:  [-2o] ... [ 2o]
             * n_:  [-4 ] [-3 ] [-2 ] [-1 ] [ 0 ] [ 1 ] [ 2 ] [ 3 ] [ 4 ]
             * i0:  [ 0 ] [ 1 ] [ 2 ] [ 3 ] [ 4 ] [ 5 ] [ 6 ] [ 7 ] [ 8 ]
             * i  ∈ [-o, ..., -1, 0, 1, ..., o]
             */
            auto D = [c](int n_, const Vector<Real> &vals){
                assert(Abs(n_)<=o);

                fix i0 = n_+2*o;

                auto val=.0;

                for(int i=-o; i<=o; ++i)
                    val += c[i+o]*vals[i0+i];

                return val;
            };

            auto kinetic_old = 0.0;
            auto grad_old    = 0.0;

            for(int k=-o; k<o; ++k) {
                kinetic_old += sqr(D(o, v_vals));
                grad_old    += sqr(D(o, h_vals));
            }
            kinetic_old *= .5/sqr(factor*δt);
            grad_old    *= .5/sqr(factor* h);
            fix pot_old     = Abs(C);


            auto kinetic_new = 0.0;
            auto grad_new    = 0.0;
            v_vals[2*o] = φ;
            h_vals[2*o] = φ;
            for(int k=-o; k<o; ++k) {
                kinetic_new += sqr(D(o, v_vals));
                grad_new    += sqr(D(o, h_vals));
            }
            kinetic_new *= .5/sqr(factor*δt);
            grad_new    *= .5/sqr(factor*h);
            fix pot_new     = Abs(φ);

            fix δS = h*δt;

            return δS*((kinetic_new-grad_new-pot_new) - (kinetic_old-grad_old-pot_old));
        };
        auto ΔS_gpt = [field](RandomSite site, NewValue new_val) {
            fix i = site.i;
            fix n = site.j;

            fix N = field->getN();
            fix M = field->getM();
            IN data = field->getSpace().getHostData();

            auto ϕ = [M, &data](int i, int n) {
                return data[i+n*M];
            };

            fix ϕₒₗ = ϕ(i,n);
            fix φ = new_val;

            fix &h2 = field->getSpace().getMetaData().geth();
            fix Δx = h2[0];
            fix Δt = h2[1];

            fix Δx2 = Δx*Δx;
            fix Δt2 = Δt*Δt;
            fix ΔxΔt = Δx*Δt;



            fix ΔS_kin = 0.5 * (  sqr(ϕ(i,n+1) - φ       )
                                + sqr(φ        - ϕ(i,n-1))
                                - sqr(ϕ(i,n+1) - ϕₒₗ     )
                                - sqr(ϕₒₗ      - ϕ(i,n-1)))
                    * ΔxΔt / Δt2;


            fix ΔS_grad = 0.5 * (  sqr(ϕ(i+1,n) - φ       )
                                 + sqr(φ        - ϕ(i-1,n))
                                 - sqr(ϕ(i+1,n) - ϕₒₗ     )
                                 - sqr(ϕₒₗ      - ϕ(i-1,n)))
                         * ΔxΔt / Δx2;

            fix ΔS_pot = -(Abs(φ) - Abs(ϕₒₗ))*ΔxΔt;

            return ΔS_kin + ΔS_grad + ΔS_pot;
        };

        setup.ΔS = ΔS_gpt;

        constexpr auto h_border_size = 1;
        constexpr auto v_border_size = 1;

        setup.sample_location = [field](){
            fix i = h_border_size + RandUtils::RandomUniformUInt()%(field->getN()-2*h_border_size);
            fix j = v_border_size + RandUtils::RandomUniformUInt()%(field->getM()-2*v_border_size);

            return RandomSite{i, j};
        };

        setup.modify = [field](RandomSite site, NewValue value){
            field->At(site.i, site.j) = value;
        };

        auto metropolis = New<R2toRMetropolis>(field, setup);

        return New<MontecarloStepper>(metropolis);
    }

} // Slab::Math