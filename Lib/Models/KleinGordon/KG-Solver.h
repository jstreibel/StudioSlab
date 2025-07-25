#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Math/Numerics/ODE/Solver/LinearStepSolver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/SignumFunction.h"
#include "KG-State.h"


namespace Slab::Models {

    using namespace Math;

    #define FieldCast(FIELD) DynamicPointerCast<Field>(FIELD)

    template<class InCategory>
    class KGSolver : public Base::LinearStepSolver {
    public:
        typedef KGState<InCategory>     FieldState;
        typedef typename FieldState::Field       Field;
        typedef typename FieldState::FieldBase   FieldBase;
        typedef typename FieldState::OutCategory OutCategory;

        using Potential =               Base::FunctionT<OutCategory, OutCategory>;
        using NonHomogenousFunc =       FieldBase;
        using NonHomogenousFunc_ptr =   TPointer<NonHomogenousFunc>;
        using Operator =                Math::Operator<Field>;

        KGSolver(Base::BoundaryConditions_ptr du,
                 TPointer<Operator> O,
                 TPointer<Potential> potential,
                 NonHomogenousFunc_ptr nonHomogenousFunc=nullptr)
        : Base::LinearStepSolver(du)
        , O(O)
        , V(potential)
        , dVDPhi(potential->diff(0))
        , f(nonHomogenousFunc)
        {    }

        ~KGSolver() override = default;

        void startStep(const Base::EquationState &state, DevFloat t, DevFloat dt) final {
            Base::LinearStepSolver::startStep(state, t, dt);

            IN kgState = dynamic_cast<const FieldState&>(state);
            this->startStep_KG(kgState, t, dt);
        }

        Base::EquationState & F(const Base::EquationState &stateIn, Base::EquationState &stateOut, DevFloat t) final {
            auto &kgStateIn  = dynamic_cast<const FieldState&>(stateIn );
            auto &kgStateOut = dynamic_cast<      FieldState&>(stateOut);

            return dynamic_cast<Base::EquationState&>(F_KG(kgStateIn, kgStateOut, t));
        }

    protected:

        virtual void
        startStep_KG(const FieldState &state, DevFloat t, DevFloat dt) {
            if(temp1 == nullptr){
                assert(temp2 == nullptr);

                temp1 = FieldCast(state.getPhi().Clone());
                temp2 = FieldCast(state.getPhi().Clone());
            }
        }

        virtual FieldState &
        F_KG(const FieldState &stateIn, FieldState &stateOut, DevFloat t) {
            const auto &ϕᵢₙ  = stateIn .getPhi();
            const auto &δₜϕᵢₙ = stateIn .getDPhiDt();
            auto       &ϕₒᵤₜ  = stateOut.getPhi();
            auto       &δₜϕₒᵤₜ = stateOut.getDPhiDt();

            auto &L = *O;
            GET Lϕ = *temp1;
            GET dVdϕ = *temp2;
            Lϕ = L*ϕᵢₙ;
            ϕᵢₙ .Apply(*dVDPhi, dVdϕ);

            /* ********
             * EQ 1
             * ********/
            ϕₒᵤₜ = δₜϕᵢₙ;

            /* ********
             * EQ 2
             * ********/
            δₜϕₒᵤₜ = Lϕ - dVdϕ;

            if(f != nullptr) {
                auto &fₑₓₜ = *f;
                δₜϕₒᵤₜ += fₑₓₜ;
            }

            return stateOut;
        }

        TPointer<Operator> O;
        TPointer<Field> temp1 = nullptr, temp2 = nullptr;
        TPointer<Potential> V;
        TPointer<Potential> dVDPhi = nullptr;
        NonHomogenousFunc_ptr f = nullptr;
    };

}




#endif // HamiltonCPU_H
