#ifndef HamiltonLangevinCPU_H
#define HamiltonLangevinCPU_H

#include "KGSolver.h"

#include "Models/KleinGordon/RtoR/KG-RtoREquationState.h"
#include "Math/Numerics/NumericalRecipe.h"


#define FRANDOM (random()/(RAND_MAX+1.0))

namespace Slab::Models::KGRtoR {
    Real xi() {
        const Real z = FRANDOM;

        const Real r = sqrt(-2.0 * log(1.0 - z));

        static Real signs[] = {-1,1};
        const Real sign = signs[random()%2];

        return sign*r;
    }

    typedef RtoR::EquationState EqState;
    typedef Fields::KleinGordon::Solver<RtoR::EquationState> KGSolver;


    class LorentzLangevin_2ndOrder : public KGSolver {
        DiscreteFunction &langevinImpulses;
        DiscreteFunction &scaledImpulses;

        Real T=0.01;
        Real alpha = 1.0;
    public:
        LorentzLangevin_2ndOrder(const NumericConfig &params, EqBoundaryCondition &du,
                                 KGSolver::PotentialFunc &potential, DiscreteFunction &langevinImpulses,
                                 DiscreteFunction &scaledImpulses) : Solver(params, du, potential),
                                                                     langevinImpulses(langevinImpulses),
                                                                     scaledImpulses(scaledImpulses) {}

        // explicit LorentzLangevin_2ndOrder(NumericParams &params, KGSolver::MyBase::EqBoundaryCondition &du,
        //                                   RtoR::Function &potential)
        // : KGSolver<RtoR::EquationState>(params, du, potential)
        // , langevinImpulses(*builder.NewFunctionArbitrary<DiscreteFunction>())
        // , scaledImpulses(  *builder.NewFunctionArbitrary<DiscreteFunction>()) { }

        void startStep(Real t, Real dt) override{
            KGSolver::startStep(t, dt);

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

        EqState &dtF(const EqState &fieldStateIn, EqState &fieldStateOut, Real t, Real dt) override {
            const DiscreteFunction &iPhi = fieldStateIn.getPhi();
            const DiscreteFunction &iDPhi = fieldStateIn.getDPhiDt();
            DiscreteFunction &oPhi = fieldStateOut.getPhi();
            DiscreteFunction &oDPhi = fieldStateOut.getDPhiDt();

            // Eq 1
            {   oPhi.SetArb(iDPhi) *= dt;   }

            // Eq 2
            {
                scaledImpulses.StoreMultiplication(langevinImpulses, sqrt(2*T/dt));

                iPhi.Laplacian(*temp1); // Laplaciano do phi de input tem seu laplaciano
                                           // calculado e o resultado vai pra dentro do temp1,
                                           // que por sua vez eh retornado;
                iPhi.Apply(*dVDPhi, temp2);

                oDPhi.StoreSubtraction(*temp1, *temp2);  // agora temp1 e temp2 estao liberados;

                oDPhi -= scaledImpulses;
                oDPhi -= iDPhi;

                oDPhi *= dt;

            }

            return fieldStateOut;
        }

    };


}


#endif // HamiltonLangevinCPU_H
