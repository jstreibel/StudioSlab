#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Phys/DifferentialEquations/EquationSolver.h"

#include "Phys/Numerics/Builder.h"


#define CLONE(func) dynamic_cast<DiscrFuncType&>(*(func.Clone()))

namespace Fields {

    namespace KleinGordon {

        template<class EqState>
        class Solver : public Slab::EquationSolverT<EqState> {
        public:
            using MyBase = Slab::EquationSolverT<EqState>;
            using PotentialFunc = Base::FunctionT<Real, Real>;
            typedef EqState::SubStateType           DiscrFuncType;

            Solver(const NumericParams &params,
                            MyBase::EqBoundaryCondition &du,
                            PotentialFunc &potential)
            : Slab::EquationSolverT<EqState>(params, du)
            , V(potential)
            , dVDPhi(potential.diff(0))
            {    }

            ~Solver() override {
                delete laplacian;
                delete dV_out;
                delete &V;
            }

            EqState &
            dtF(const EqState &stateIn, EqState &stateOut, Real t, Real dt) override {
                if(laplacian == nullptr){
                    assert(dV_out == nullptr);

                    laplacian = (DiscrFuncType*)stateIn.getPhi().Clone();
                    dV_out    = (DiscrFuncType*)stateIn.getPhi().Clone();
                }

                const auto &iPhi = stateIn.getPhi();
                const auto &iDPhi = stateIn.getDPhiDt();
                auto &oPhi = stateOut.getPhi();
                auto &oDPhi = stateOut.getDPhiDt();

                // Eq 1
                { oPhi.SetArb(iDPhi) *= dt; }

                // Eq 2
                {
                    iPhi.Laplacian(*laplacian);
                    iPhi.Apply(*dVDPhi, *dV_out);

                    oDPhi.StoreSubtraction(*laplacian, *dV_out) *= dt;
                }

                return stateOut;
            }

            static bool isDissipating() { return false; }

        protected:
            DiscrFuncType *laplacian = nullptr
                        , *dV_out    = nullptr;
            PotentialFunc &V;
            PotentialFunc::Ptr dVDPhi= nullptr;
        };

}

}


#endif // HamiltonCPU_H
