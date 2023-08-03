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

            explicit Solver(const NumericParams &params,
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
            dtF(const EqState &fieldStateIn, EqState &fieldStateOut, Real t, Real dt) override {
                if(laplacian == nullptr){
                    assert(dV_out == nullptr);

                    laplacian = (DiscrFuncType*)fieldStateIn.getPhi().Clone();
                    dV_out    = (DiscrFuncType*)fieldStateIn.getPhi().Clone();
                }

                const auto &iPhi = fieldStateIn.getPhi();
                const auto &iDPhi = fieldStateIn.getDPhiDt();
                auto &oPhi = fieldStateOut.getPhi();
                auto &oDPhi = fieldStateOut.getDPhiDt();

                // Eq 1
                { oPhi.SetArb(iDPhi) *= dt; }

                // Eq 2
                {
                    iPhi.Laplacian(*laplacian);
                    iPhi.Apply(*dVDPhi, *dV_out);

                    oDPhi.StoreSubtraction(*laplacian, *dV_out) *= dt;
                }

                return fieldStateOut;
            }

            static bool isDissipating() { return false; }

        protected:
            DiscrFuncType *laplacian = nullptr
                        , *dV_out    = nullptr;
            PotentialFunc &V;
            PotentialFunc::Ptr dVDPhi;
        };

}

}


#endif // HamiltonCPU_H
