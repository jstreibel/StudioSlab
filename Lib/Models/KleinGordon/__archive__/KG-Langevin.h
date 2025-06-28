#ifndef HamiltonLangevinCPU_H
#define HamiltonLangevinCPU_H

#include "Models/KleinGordon/KG-Solver.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREquationState.h"
#include "Math/Numerics/NumericalRecipe.h"


#define FRANDOM (random()/(RAND_MAX+1.0))

namespace Slab::Models::KGRtoR {
    DevFloat xi() {
        const DevFloat z = FRANDOM;

        const DevFloat r = sqrt(-2.0 * log(1.0 - z));

        static DevFloat signs[] = {-1,1};
        const DevFloat sign = signs[random()%2];

        return sign*r;
    }

    typedef RtoR::EquationState EqState;
    typedef Fields::KleinGordon::Solver<RtoR::EquationState> KGSolver;


    class LorentzLangevin_2ndOrder : public KGSolver {
        NumericFunction &langevinImpulses;
        NumericFunction &scaledImpulses;

        DevFloat T=0.01;
        DevFloat alpha = 1.0;
    public:
        LorentzLangevin_2ndOrder(const NumericConfig &params, EqBoundaryCondition &du,
                                 KGSolver::PotentialFunc &potential, NumericFunction &langevinImpulses,
                                 NumericFunction &scaledImpulses) : Solver(params, du, potential),
                                                                     langevinImpulses(langevinImpulses),
                                                                     scaledImpulses(scaledImpulses) {}

        // explicit LorentzLangevin_2ndOrder(NumericParams &params, KGSolver::MyBase::EqBoundaryCondition &du,
        //                                   RtoR::Function &potential)
        // : KGSolver<RtoR::EquationState>(params, du, potential)
        // , langevinImpulses(*builder.NewFunctionArbitrary<NumericFunction>())
        // , scaledImpulses(  *builder.NewFunctionArbitrary<NumericFunction>()) { }

        void startStep(DevFloat t, DevFloat dt) override{
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

        void setTemperature(DevFloat value) {T = value;}

        EqState &dtF(const EqState &fieldStateIn, EqState &fieldStateOut, DevFloat t, DevFloat dt) override {
            const NumericFunction &iPhi = fieldStateIn.getPhi();
            const NumericFunction &iDPhi = fieldStateIn.getDPhiDt();
            NumericFunction &oPhi = fieldStateOut.getPhi();
            NumericFunction &oDPhi = fieldStateOut.getDPhiDt();

            // Eq 1
            {   oPhi.SetArb(iDPhi) *= dt;   }

            // Eq 2
            {
                scaledImpulses.StoreScalarMultiplication(langevinImpulses, sqrt(2 * T / dt));

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
