#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Phys/DifferentialEquations/EquationSolver.h"

#include "Phys/Numerics/Builder.h"


#define FType(a) typename EqState::a

namespace Phys {

    namespace Gordon {

        template<class EqState>
        class GordonSolverT : public Base::EquationSolverT<EqState> {
        protected:
            typedef Base::FunctionT<
                    FType(FunctionType::OutCategory),
                    FType(FunctionType::OutCategory)>
                    TargetToTargetFunction;
        public:
            explicit GordonSolverT(const EqState &phi_0, TargetToTargetFunction &potential)
            : phi_0(phi_0)
            , laplacian(*phi_0.getPhi().Clone())
            , dV_out(   *phi_0.getPhi().Clone())
            , V(potential)
            , dVDPhi(potential.diff(0))
            {    }

            ~GordonSolverT() override {
                delete &laplacian;
                delete &dV_out;
                delete &V;
            }

            virtual EqState &applyBC(EqState &fieldStateOut, Real t, Real dt) {
                if(t==0) fieldStateOut.set(phi_0);

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
            const EqState &phi_0;
            FType(FunctionArbitraryType) &laplacian, &dV_out;
            TargetToTargetFunction &V;
            TargetToTargetFunction::Ptr dVDPhi;
        };

}


#endif // HamiltonCPU_H
