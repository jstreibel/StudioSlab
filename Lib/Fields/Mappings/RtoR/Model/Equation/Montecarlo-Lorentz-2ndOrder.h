//
// Created by joao on 3/13/23.
//

#ifndef STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H
#define STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H

#include "Phys/DifferentialEquations/2nd-Order/Lorentz-2ndOrder.h"

#include "Phys/Numerics/Allocator.h"

#include "../RtoRFieldState.h"


#define FType(a) typename FieldState::a

#define FRANDOM (random()/(RAND_MAX+1.0))

namespace RtoR {

    typedef Base::Lorentz_2ndOrder<RtoR::FieldState> LorentzInvariant;


    class MontecarloLangevin_2ndOrder : public LorentzInvariant {
        Real T=0.01;

    public:
        explicit MontecarloLangevin_2ndOrder(RtoR::Function &potential): LorentzInvariant(potential) { }

        void startStep(Real t, Real dt) override{
            Equation::startStep(t, dt);
        }

        void setTemperature(Real value) {T = value;}

        FieldState &dtF(const FieldState &fieldStateIn, FieldState &fieldStateOut, Real t, Real dt) override {
            const auto &iPhi = fieldStateIn.getPhi();
            const auto &iDPhi = fieldStateIn.getDPhiDt();
            auto &oPhi = fieldStateOut.getPhi();
            auto &oDPhi = fieldStateOut.getDPhiDt();

            // Eq 1
            {   oPhi.SetArb(iDPhi) *= dt;   }

            // Eq 2
            {
                iPhi.Laplacian(laplacian);
                iPhi.Apply(dVDPhi, dV);

                oDPhi.StoreSubtraction(laplacian, dV) *= dt;
            }

            return fieldStateOut;
        }

    };

}

#endif //STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H
