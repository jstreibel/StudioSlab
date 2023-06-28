#ifndef HamiltonLangevinCPU_H
#define HamiltonLangevinCPU_H

#include "Phys/DifferentialEquations/2nd-Order/GordonSystem.h"

#include "Phys/Numerics/Allocator.h"

#include "Mappings/RtoR/Model/RtoRFieldState.h"


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

    typedef Base::Gordon<RtoR::FieldState> LorentzInvariant;


    class LorentzLangevin_2ndOrder : public LorentzInvariant {



        ArbitraryFunction &langevinImpulses;
        ArbitraryFunction &scaledImpulses;

        Real T=0.01;
        Real alpha = 1.0;
    public:

        explicit LorentzLangevin_2ndOrder(RtoR::Function &potential): LorentzInvariant(potential),
                                                                      langevinImpulses(*(ArbitraryFunction*)Numerics::Allocator::
                                                                      getInstance().newFunctionArbitrary()),
                                                                      scaledImpulses(*(ArbitraryFunction*)Numerics::Allocator::
                                                                      getInstance().newFunctionArbitrary()) { }

        void startStep(Real t, Real dt) override{
            DifferentialEquation::startStep(t, dt);

            ComputeLangevin();
        }

        void ComputeLangevin(){
            auto &space = langevinImpulses.getSpace();
            assert(space.getDim().getNDim() == 1);

            auto &X = space.getHostData();

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

                iPhi.Laplacian(laplacian); // Laplaciano do phi de input tem seu laplaciano
                                           // calculado e o resultado vai pra dentro do temp1,
                                           // que por sua vez eh retornado;
                iPhi.Apply(dVDPhi, dV);

                oDPhi.StoreSubtraction(laplacian, dV);  // agora temp1 e temp2 estao liberados;



                oDPhi -= scaledImpulses;
                oDPhi -= iDPhi;

                oDPhi *= dt;

            }

            return fieldStateOut;
        }

    };

}


#endif // HamiltonLangevinCPU_H
