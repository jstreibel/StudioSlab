#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Math/DifferentialEquations/Solver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/SignumFunction.h"
#include "KGState.h"


namespace Slab::Models {

    using namespace Math;

    template<class InCategory>
    class KGSolver : public Base::Solver {
    public:
        typedef KGState<InCategory>     FieldState;
        typedef FieldState::Field       Field;
        typedef FieldState::FieldBase   FieldBase;
        typedef FieldState::OutCategory OutCategory;

        using Potential =               Base::FunctionT<OutCategory, OutCategory>;
        using NonHomogenousFunc =       FieldBase;
        using NonHomogenousFunc_ptr =   Pointer<NonHomogenousFunc>;
        using Operator =                Math::Operator<Field>;

        KGSolver(const NumericConfig &params,
               Base::BoundaryConditions_ptr du,
               const Pointer<Operator> & O,
               const Pointer<Potential>& potential,
               NonHomogenousFunc_ptr nonHomogenousFunc=NonHomogenousFunc_ptr())
        : Base::Solver(params, du)
        , O(O)
        , V(potential)
        , dVDPhi(potential->diff(0))
        , f(nonHomogenousFunc)
        {    }

        ~KGSolver() override = default;

        void startStep(const Base::EquationState &state, Real t, Real dt) final {
            Base::Solver::startStep(state, t, dt);

            IN kgState = dynamic_cast<const FieldState&>(state);
            this->startStep_KG(kgState, t, dt);
        }

        Base::EquationState &
        dtF(const Base::EquationState &stateIn, Base::EquationState &stateOut, Real t) final {
            auto &kgStateIn  = dynamic_cast<const FieldState&>(stateIn );
            auto &kgStateOut = dynamic_cast<      FieldState&>(stateOut);

            return dynamic_cast<Base::EquationState&>(dtF_KG(kgStateIn, kgStateOut, t));
        }

    protected:

        virtual void
        startStep_KG(const FieldState &state, Real t, Real dt) {
            if(temp1 == nullptr){
                assert(temp2 == nullptr);

                temp1 = DynamicPointerCast<Field>(state.getPhi().Clone());
                temp2 = DynamicPointerCast<Field>(state.getPhi().Clone());
            }
        }

        virtual FieldState &
        dtF_KG(const FieldState &stateIn, FieldState &stateOut, Real t) {
            const auto &iPhi  = stateIn .getPhi();
            const auto &iDPhi = stateIn .getDPhiDt();
            auto       &oPhi  = stateOut.getPhi();
            auto       &oDPhi = stateOut.getDPhiDt();

            // Eq 1
            {
                oPhi.SetArb(iDPhi);
            }

            // Eq 2
            {
                auto &L = *O;

                GET laplacianOutput = *temp1;
                GET dVdphi_out = *temp2;

                auto Lϕ = L*iPhi;
                laplacianOutput = Lϕ;
                // iPhi.Laplacian(laplacianOutput);
                iPhi.Apply(*dVDPhi, dVdphi_out);

                oDPhi = laplacianOutput - dVdphi_out;

                if(f != nullptr) oDPhi += *f;
            }

            return stateOut;
        }

        Pointer<Operator> O;
        Pointer<Field> temp1 = nullptr, temp2 = nullptr;
        Pointer<Potential> V;
        Pointer<Potential> dVDPhi = nullptr;
        NonHomogenousFunc_ptr f = nullptr;
    };

}




#endif // HamiltonCPU_H
