//
// Created by joao on 3/13/23.
//

#ifndef STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H
#define STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H

#include "Phys/DifferentialEquations/2nd-Order/Lorentz-2ndOrder.h"

#include "Phys/Numerics/Allocator.h"

#include "../RtoRFieldState.h"

#include "Phys/Thermal/Utils/RandUtils.h"
#include "Phys/Thermal/Utils/ThermoUtils.h"


#define FType(a) typename FieldState::a

#define FRANDOM (random()/(RAND_MAX+1.0))

namespace RtoR {

    typedef Base::Lorentz_2ndOrder<RtoR::FieldState> LorentzInvariant;


    class MontecarloLangevin_2ndOrder : public LorentzInvariant {
        Real T=1.e6;
        unsigned accepted = 0;

        double deltaE(VecFloat_I &phi, const int site, const double newVal, const double h){
            const auto N = phi.size();

            const auto C = phi[site];
            const auto L = site-1<0  ? phi[N-1] : phi[site-1];
            const auto R = site+1>=N ? phi[0]   : phi[site+1];

            const auto v = newVal;

            return (1/h) * (C-v) * (v - L + C - R) + h*(V(C) - V(v));
        }

        bool shouldAccept(double deltaE){
            if(deltaE<0) return true;

            const double r = RandUtils::random01();

            const double z = ThermoUtils::BoltzmannWeight(T, deltaE);

            return (r<z);
        }
    public:
        explicit MontecarloLangevin_2ndOrder(RtoR::Function &potential): LorentzInvariant(potential) { }

        void startStep(Real t, Real dt) override{
            Equation::startStep(t, dt);
            accepted = 0;
        }

        void setTemperature(Real value) {T = value;}
        Real getTemperature() { return T; }


        FieldState &dtF(const FieldState &null, FieldState &fieldState, Real t, Real dt) override {
            auto &phi = fieldState.getPhi();
            auto &dPhidt = fieldState.getDPhiDt();

            auto N = phi.N;
            auto h = phi.getSpace().geth();
            auto &X = phi.getSpace().getX();

            for (int ssf = 0; ssf < N; ++ssf) {
                // sorteio usando prob. (uniforme) do sitio estar na linha i:  P_i=1/L
                const int i = RandUtils::RandInt() % N;
                const Real v = X[i];
                const Real newVal = v + RandUtils::random(-.1,.1);

                const double deltaE = this->deltaE(X, i, newVal, h);

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
