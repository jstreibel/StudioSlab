#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Phys/DifferentialEquations/EquationSolver.h"

#include "Phys/Numerics/Builder.h"
#include "Mappings/RtoR/Model/FunctionsCollection/SignumFunction.h"


#define CLONE(func) dynamic_cast<DiscrFuncType&>(*(func.Clone()))

namespace Fields {

    namespace KleinGordon {

        template<class EqState>
        class Solver : public Slab::EquationSolverT<EqState> {
        public:
            using MyBase = Slab::EquationSolverT<EqState>;
            using PotentialFunc = Base::FunctionT<Real, Real>;

            typedef EqState::SubStateType           DiscrFuncType;

            Solver(const NumericConfig &params,
                            MyBase::EqBoundaryCondition &du,
                            PotentialFunc &potential)
            : Slab::EquationSolverT<EqState>(params, du)
            , V(potential)
            , dVDPhi(potential.diff(0))
            {    }

            ~Solver() override {
                delete temp1;
                delete temp2;
                delete &V;
            }

            EqState &
            dtF(const EqState &stateIn, EqState &stateOut, Real t, Real dt) override {
                if(temp1 == nullptr){
                    assert(temp2 == nullptr);

                    temp1 = (DiscrFuncType*)stateIn.getPhi().Clone();
                    temp2    = (DiscrFuncType*)stateIn.getPhi().Clone();
                }
                #pragma omp barrier

                const auto &iPhi = stateIn.getPhi();
                const auto &iDPhi = stateIn.getDPhiDt();
                auto &oPhi = stateOut.getPhi();
                auto &oDPhi = stateOut.getDPhiDt();

                // Eq 1
                { oPhi.SetArb(iDPhi) *= dt; }

                // Eq 2
                {
                    GET laplacian = *temp1;
                    GET dVdphi_out = *temp2;

                    iPhi.Laplacian(laplacian);
                    iPhi.Apply(*dVDPhi, dVdphi_out);

                    oDPhi.StoreSubtraction(*temp1, *temp2) *= dt;
                }

                return stateOut;
            }

            static bool isDissipating() { return false; }

        protected:
            DiscrFuncType *temp1 = nullptr
                        , *temp2 = nullptr;
            PotentialFunc &V;
            PotentialFunc::Ptr dVDPhi= nullptr;
        };

}

}


#endif // HamiltonCPU_H
