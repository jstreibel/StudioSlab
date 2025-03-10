//
// Created by joao on 3/13/23.
//

#ifndef STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H
#define STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H

#include "Models/KleinGordon/KG-Solver.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

#include "Utils/RandUtils.h"
#include "Math/Thermal/ThermoUtils.h"


#define FType(a) typename FieldState::a

#define FRANDOM (random()/(RAND_MAX+1.0))

namespace Slab::Models::KGRtoR {

    typedef Fields::KleinGordon::GordonSystem<RtoR::FieldState> LorentzInvariant;


    class MontecarloLangevin_2ndOrder : public LorentzInvariant {
        Real T=1.e-3;
        unsigned accepted = 0;

        Real E(const NumericFunction &phi){
            const auto h = phi.getSpace().GetHeight();
            const auto inv_2h = .5/h;
            const auto &X = phi.getSpace().getHostData();
            const auto &N = phi.N;

            Real E_h = .0;
            for(auto i=0; i<N; ++i){
                const auto &x_left = X[i==0   ? N-1 : i-1];
                const auto &x = X[i];
                const auto &x_right = X[i==N-1 ?   0 : i+1];

                const auto dx = (x_right-x_left)*inv_2h;
                const auto absx = abs(x);

                E_h += .5*dx*dx + V(x);
            }

            return h*E_h;
        }

        NumericFunction *temp;
        Real deltaE(NumericFunction &phi, const int site, const Real newVal, const Real h){
            NumericFunction &phiNew = *temp;

            phiNew.SetArb(phi);

            phiNew.getSpace().getHostData()[site] = newVal;

            const auto deltaE = E(phiNew) - E(phi);

            return deltaE;


            //const auto N = phi.size();
            //const auto C = phi[site];
            //const auto L = site-1<0  ? phi[N-1] : phi[site-1];
            //const auto R = site+1>=N ? phi[0]   : phi[site+1];
            //const auto v = newVal;
            //return (1/h) * (C-v) * (v - L + C - R) + h*(V(C) - V(v));
        }

        bool shouldAccept(Real deltaE){
            if(deltaE<0) return true;

            const Real r = RandUtils::RandomUniform01();

            const Real z = ThermoUtils::BoltzmannWeight(T, deltaE);

            return (r<z);
        }
    public:
        explicit MontecarloLangevin_2ndOrder(Core::Simulation::Builder &builder, RtoR::Function &potential)
            : LorentzInvariant(potential), temp(Numerics::Allocator::NewFunctionArbitrary<NumericFunction>()) { }

        void startStep(Real t, Real dt) override{
            DifferentialEquation::startStep(t, dt);
            accepted = 0;
        }

        void setTemperature(Real value) {T = value;}
        Real getTemperature() { return T; }


        FieldState &dtF(const FieldState &null, FieldState &fieldState, Real t, Real dt) override {
            auto &phi = fieldState.getPhi();
            auto &dPhidt = fieldState.getDPhiDt();

            auto N = phi.N_modes;
            auto h = phi.getSpace().GetHeight();
            auto &X = phi.getSpace().getHostData();

            for (int ssf = 0; ssf < N; ++ssf) {
                // sorteio usando prob. (uniforme) do sitio estar na linha i:  P_i=1/L
                const int i = RandUtils::RandomUniformInt() % N;

                const Real v = X[i];
                const Real newVal = v + RandUtils::RandomUniform(-.5, .5);

                const Real deltaE = this->deltaE(phi, i, newVal, h);

                if (shouldAccept(deltaE)) {
                    X[i] = newVal;
                    accepted++;
                }
            }


            return fieldState;
        }

    };

}

#endif //STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H
