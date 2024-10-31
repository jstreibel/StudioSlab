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

#include "Math/Toolset/NativeFunctions.h"


namespace Slab::Math {

    // inline Real sqr(const Real &v){ return v*v; }

    // Don't touch:
    #define DONT_REGISTER false
    #define DO_REGISTER true

    auto R2toRMetropolisRecipe::getField() -> Pointer<R2toR::NumericFunction_CPU> {
        if(field_data == nullptr){
            fix N=20, M=20;
            fix x_min=-1.2, y_min=1.2;
            fix Lx=-2*x_min, Ly=Lx; ///(Real(N)/Real(M));

            field_data = DataAlloc<R2toR::NumericFunction_CPU>("Stochastic field",
                                                               N, M,
                                                               x_min, y_min,
                                                               Lx/Real(N), Ly/Real(M));
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
        constexpr auto δϕₘₐₓ = 5e-2;

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

        auto ΔS_fw_deriv     = [field](RandomSite site, NewValue new_val) {
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
        auto ΔS_sym_deriv    = [field](RandomSite site, NewValue new_val) {
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
        auto ΔS_sym_deriv_O2 = [field](RandomSite site, NewValue new_val) {
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
        auto ΔS_gpt          = [field](RandomSite site, NewValue new_val) {
            fix i = site.i;
            fix n = site.j;

            fix N = field->getN();
            IN data = field->getSpace().getHostData();

            auto ϕ = [N, &data](int i, int n) {
                return data[i+n*N];
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


            fix ΔS_grad =-0.5 * (  sqr(ϕ(i+1,n) - φ       )
                                 + sqr(φ        - ϕ(i-1,n))
                                 - sqr(ϕ(i+1,n) - ϕₒₗ     )
                                 - sqr(ϕₒₗ      - ϕ(i-1,n)))
                         * ΔxΔt / Δx2;

            fix ΔS_pot = -(Abs(φ) - Abs(ϕₒₗ))*ΔxΔt;

            return ΔS_kin + ΔS_grad + ΔS_pot;
        };
        auto ΔSₑ_gpt         = [field](RandomSite site, NewValue new_val) {
            fix i = site.i;
            fix n = site.j;

            fix N = field->getN();
            IN data = field->getSpace().getHostData();

            auto ϕ = [N, &data](int i, int n) {
                return data[i+n*N];
            };

            fix ϕₒₗ = ϕ(i,n);
            fix φ = new_val;

            fix &h2 = field->getSpace().getMetaData().geth();
            fix Δx = h2[0];
            fix Δt = h2[1];

            fix Δx2 = Δx*Δx;
            fix Δt2 = Δt*Δt;
            fix ΔxΔt = Δx*Δt;


            fix ΔE_kin = 0.5 * (  sqr(ϕ(i,n+1) - φ       )
                                  + sqr(φ        - ϕ(i,n-1))
                                  - sqr(ϕ(i,n+1) - ϕₒₗ     )
                                  - sqr(ϕₒₗ      - ϕ(i,n-1)))
                         * ΔxΔt / Δt2;


            fix ΔE_grad = 0.5 * (  sqr(ϕ(i+1,n) - φ       )
                                   + sqr(φ        - ϕ(i-1,n))
                                   - sqr(ϕ(i+1,n) - ϕₒₗ     )
                                   - sqr(ϕₒₗ      - ϕ(i-1,n)))
                          * ΔxΔt / Δx2;

            fix ΔE_pot = (Abs(φ) - Abs(ϕₒₗ))*ΔxΔt;

            return ΔE_kin + ΔE_grad + ΔE_pot;
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

            fix Δx2 = Δx*Δx;
            fix Δt2 = Δt*Δt;

            auto sign = Slab::Math::SIGN<Real>;

            Vector<RandomSite> affected_sites = {
                    {i, n},
                    {i+1, n}, {i - 1, n},
                    {i, n + 1}, {i, n - 1}};

            // fix 𝜕ₓ²ϕ =

            auto δSδϕ_old = .0;
            for(auto s : affected_sites) {
                fix i = s.i;
                fix n = s.j;

                δSδϕ_old += sqr((ϕ(i  ,n+1) - 2.*ϕ(i,n) + ϕ(i  ,n-1))/Δt2
                              - (ϕ(i+1,n  ) - 2.*ϕ(i,n) + ϕ(i-1,n  ))/Δx2
                              + sign(ϕ(i,n)));
            }

            field->At(i,n) = φ;
            auto δSδϕ_new = .0;
            for(auto s : affected_sites) {
                fix i = s.i;
                fix n = s.j;

                δSδϕ_new += sqr((ϕ(i  ,n+1) - 2.*ϕ(i,n) + ϕ(i  ,n-1))/Δt2
                              - (ϕ(i+1,n  ) - 2.*ϕ(i,n) + ϕ(i-1,n  ))/Δx2
                                + sign(ϕ(i,n)));


            }
            field->At(i,n) = ϕₒₗ;

            return δSδϕ_new-δSδϕ_old;
        };
        auto Δ_δSδϕ_old      = [field](RandomSite site, NewValue new_val) {
            fix i = site.i;
            fix n = site.j;

            auto ϕ = [field](int i_, int n_) {
                return field->At(i_, n_);
            };

            fix ϕₒₗ = ϕ(i,n);
            fix φ = new_val;

            fix &h2 = field->getSpace().getMetaData().geth();
            fix Δx = h2[0];
            fix Δt = h2[1];

            fix Δx2 = Δx*Δx;
            fix Δt2 = Δt*Δt;
            fix ΔtΔx = Δt*Δx;

            auto sign = Slab::Math::SIGN<Real>;

            // fix 𝜕ₓ²ϕ =

            double δSδϕ_old = ((ϕ(i  ,n+1) - 2.*ϕₒₗ + ϕ(i  ,n-1))/Δt2
                             - (ϕ(i+1,n  ) - 2.*ϕₒₗ + ϕ(i-1,n  ))/Δx2
                             + sign(ϕₒₗ)) * ΔtΔx;


            double δSδϕ_new = ((ϕ(i  ,n+1) - 2.*φ   + ϕ(i  ,n-1))/Δt2
                             - (ϕ(i+1,n  ) - 2.*φ   + ϕ(i-1,n  ))/Δx2
                             + sign(φ)) * ΔtΔx;

            return Abs(δSδϕ_new)-Abs(δSδϕ_old);
        };

        setup.Δ_δSδϕ = Δ_δSδϕ;

        fix h_border_size = field->getN()/4;
        fix v_border_size = field->getM()/4;;

        setup.sample_location = [field,h_border_size,v_border_size](){
            fix i = h_border_size + RandUtils::RandomUniformUInt() % (field->getN() - 2 * h_border_size);
            fix j = v_border_size + RandUtils::RandomUniformUInt() % (field->getM() - 2 * v_border_size);

            return RandomSite{i, j};
        };

        setup.modify = [field](RandomSite site, NewValue value){
            field->At(site.i, site.j) = value;
        };

        auto metropolis = New<R2toRMetropolis>(field, setup);

        return New<MontecarloStepper>(metropolis);
    }

} // Slab::Math