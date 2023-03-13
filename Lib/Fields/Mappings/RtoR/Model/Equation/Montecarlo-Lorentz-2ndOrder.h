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



        ArbitraryFunction &langevinImpulses;
        ArbitraryFunction &scaledImpulses;

        Real T=0.01;
        Real alpha = 1.0;
    public:

        explicit MontecarloLangevin_2ndOrder(RtoR::Function &potential): LorentzInvariant(potential),
                                                                      langevinImpulses(*(ArbitraryFunction*)Allocator::
                                                                      getInstance().newFunctionArbitrary()),
                                                                      scaledImpulses(*(ArbitraryFunction*)Allocator::
                                                                      getInstance().newFunctionArbitrary()) { }

        void startStep(Real t, Real dt) override{
            Equation::startStep(t, dt);
        }

        void setTemperature(Real value) {T = value;}

        FieldState &dtF(const FieldState &fieldStateIn, FieldState &fieldStateOut, Real t, Real dt) override {
            const ArbitraryFunction &iPhi = fieldStateIn.getPhi();
            const ArbitraryFunction &iDPhi = fieldStateIn.getDPhiDt();
            ArbitraryFunction &oPhi = fieldStateOut.getPhi();
            ArbitraryFunction &oDPhi = fieldStateOut.getDPhiDt();

            // Eq 1
            {   oPhi.SetArb(iDPhi) *= dt;   }

            // Eq 2
            {
                iPhi.Laplacian(laplacian); // Laplaciano do phi de input tem seu laplaciano
                // calculado e o resultado vai pra dentro do temp1,
                // que por sua vez eh retornado;
                iPhi.Apply(dVDPhi, dV);

                oDPhi.StoreSubtraction(laplacian, dV);  // agora temp1 e temp2 estao liberados;

                oDPhi -= iDPhi;

                oDPhi *= dt;

            }

            return fieldStateOut;
        }

    };

}

#endif //STUDIOSLAB_MONTECARLO_LORENTZ_2NDORDER_H
