#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Phys/DifferentialEquations/EquationSolver.h"

#include "Phys/Numerics/Builder.h"


#define CLONE(func) dynamic_cast<DiscrFuncType&>(*(func.Clone()))

namespace Phys {

    namespace Gordon {

        template<class EqState>
        class GordonSolverT : public Slab::EquationSolverT<EqState> {
        public:
            typedef Base::FunctionT<Real, Real>     PotentialFunc;
            typedef EqState::SubStateType           DiscrFuncType;

            explicit GordonSolverT(const NumericParams &params,
                                   const EqState &u_0,
                                   PotentialFunc &potential)
            : Slab::EquationSolverT<EqState>(params, u_0)
            , laplacian(CLONE(u_0.getPhi()))
            , dV_out(   CLONE(u_0.getPhi()))
            , V(potential)
            , dVDPhi(potential.diff(0))
            {    }

            ~GordonSolverT() override {
                delete &laplacian;
                delete &dV_out;
                delete &V;
            }

            auto NewEqState() const -> EqState * override { return this->u_0.clone(); }

            virtual EqState &applyBC(EqState &fieldStateOut, Real t, Real dt) override {
                if(t==0) fieldStateOut.set(this->u_0);

                return fieldStateOut;
            }

            EqState &
            dtF(const EqState &fieldStateIn, EqState &fieldStateOut, Real t, Real dt) override {
                applyBC(fieldStateOut, t, dt);

                const auto &iPhi = fieldStateIn.getPhi();
                const auto &iDPhi = fieldStateIn.getDPhiDt();
                auto &oPhi = fieldStateOut.getPhi();
                auto &oDPhi = fieldStateOut.getDPhiDt();

                // Eq 1
                { oPhi.SetArb(iDPhi) *= dt; }

                // Eq 2
                {
                    iPhi.Laplacian(laplacian);
                    iPhi.Apply(*dVDPhi, dV_out);

                    oDPhi.StoreSubtraction(laplacian, dV_out) *= dt;
                }

                return fieldStateOut;
            }

            static bool isDissipating() { return false; }

        protected:
            DiscrFuncType &laplacian, &dV_out;
            PotentialFunc &V;
            PotentialFunc::Ptr dVDPhi;
        };

}

}


#endif // HamiltonCPU_H
