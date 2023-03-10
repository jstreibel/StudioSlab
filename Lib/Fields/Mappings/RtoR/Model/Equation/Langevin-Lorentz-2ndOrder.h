#ifndef HamiltonLangevinCPU_H
#define HamiltonLangevinCPU_H

#include "Phys/DifferentialEquations/2nd-Order/Lorentz-2ndOrder.h"

#include "Phys/Numerics/Allocator.h"

#include "../RtoRFieldState.h"


#define FType(a) typename FieldState::a

#define FRANDOM (random()/(RAND_MAX+1.0))

namespace RtoR {
    Real xi() {
        const Real z = FRANDOM;

        const Real r = sqrt(-2.0 * log(1.0 - z));

        static Real signs[] = {-1,1};
        const Real sign = signs[random()%2];

        return sign*r;
    }

    typedef Base::Lorentz_2ndOrder<RtoR::FieldState> LorentzInvariant;


    class LorentzLangevin_2ndOrder : public LorentzInvariant {

        ArbitraryFunction &langevinImpulses;
        ArbitraryFunction &scaledImpulses;

        Real T=0.01;
        Real eta = 1.0;
    public:

        explicit LorentzLangevin_2ndOrder(RtoR::Function &potential): LorentzInvariant(potential),
                                                                      langevinImpulses(*(ArbitraryFunction*)Allocator::
                                                                      getInstance().newFunctionArbitrary()),
                                                                      scaledImpulses(*(ArbitraryFunction*)Allocator::
                                                                      getInstance().newFunctionArbitrary()) { }

        void startStep(Real t, Real dt) override{
            Equation::startStep(t, dt);

            ComputeLangevin();
        }

        void ComputeLangevin(){
            auto &space = langevinImpulses.getSpace();
            assert(space.getDim().getNDim() == 1);

            auto &X = space.getX();

            for (auto &x : X)
                x = xi();
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
                scaledImpulses.StoreMultiplication(langevinImpulses, sqrt(2*T/dt));

                auto &laplacian = iPhi.Laplacian(temp1);
                auto &dV = iPhi.Apply(dVDPhi, temp2);

                oDPhi.StoreSubtraction(laplacian, dV);

                oDPhi -= scaledImpulses;

                oDPhi -= iDPhi;

                oDPhi *= dt;

            }

            return fieldStateOut;
        }

    };

}


#endif // HamiltonLangevinCPU_H
