//
// Created by joao on 12/08/23.
//

#ifndef STUDIOSLAB_LANGEVINKGSOLVER_H
#define STUDIOSLAB_LANGEVINKGSOLVER_H

#include "Models/KleinGordon/KGSolver.h"
#include "Models/KleinGordon/KGState.h"

#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"

#include "Phys/Numerics/VoidBuilder.h"

#include "Mappings/RtoR/Model/RtoRDiscreteFunction.h"
#include "Mappings/RtoR/Model/FunctionsCollection/SignumFunction.h"


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

    typedef Fields::KleinGordon::Solver<RtoR::EquationState> KGSolver;


    class LangevinKGSolver : public KGSolver {
        DiscreteFunction *langevinImpulses;
        DiscreteFunction *scaledImpulses;

        Real T=0.01;
        Real alpha = 1.0;
    public:

        /*
        explicit LangevinKGSolver(const NumericParams &numericParams, KGSolver::MyBase::EqBoundaryCondition &boundary)
        : KGSolver(numericParams, boundary, *(new RtoR::SignumFunction())) { }
         */

        void startStep(Real t, Real dt) override{
            KGSolver::startStep(t, dt);

            ComputeImpulses();
        }

        void ComputeImpulses(){
            if(langevinImpulses == nullptr) return;

            auto &space = langevinImpulses->getSpace();
            assert(space.getDim().getNDim() == 1);

            auto &X = space.getHostData();

            for (auto &x : X)
                x = xi();
        }

        void setTemperature(Real value) {T = value;}

        EqState &
        dtF(const EqState &stateIn, EqState &stateOut, Real t, Real dt) override {
            if(langevinImpulses == nullptr){
                assert(scaledImpulses == nullptr);

                langevinImpulses = (EqState::SubStateType*)stateIn.getPhi().Clone();
                scaledImpulses   = (EqState::SubStateType*)stateIn.getPhi().Clone();
            }

            KGSolver::dtF(stateIn, stateOut, t, dt);

            const DiscreteFunction &iPhi = stateIn.getPhi();
            const DiscreteFunction &iDPhi = stateIn.getDPhiDt();
            DiscreteFunction &oPhi = stateOut.getPhi();
            DiscreteFunction &oDPhi = stateOut.getDPhiDt();

            // Eq 1
            {   oPhi.SetArb(iDPhi) *= dt;   }

            // Eq 2
            {

                //iPhi.Laplacian(*laplacian); // Laplaciano do phi de input tem seu laplaciano
                                            // calculado e o resultado vai pra dentro do temp1,
                                            // que por sua vez eh retornado;
                //iPhi.Apply(*dVDPhi, *dV_out);
                //oDPhi.StoreSubtraction(*laplacian, *dV_out);  // agora temp1 e temp2 estao liberados;
                // oDPhi -= iDPhi;

                scaledImpulses->StoreMultiplication(*langevinImpulses, sqrt(2*T/dt));

                oDPhi -= (*scaledImpulses*=dt);

                // oDPhi *= dt;

            }

            return stateOut;
        }

    };


}

#endif //STUDIOSLAB_LANGEVINKGSOLVER_H
