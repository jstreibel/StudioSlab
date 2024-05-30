#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Math/Numerics/Solver/Solver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/SignumFunction.h"
#include "KGState.h"


namespace Slab::Models {

    using namespace Math;

    #define FieldCast(FIELD) DynamicPointerCast<Field>(FIELD)

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

                temp1 = FieldCast(state.getPhi().Clone());
                temp2 = FieldCast(state.getPhi().Clone());
            }
        }

        virtual FieldState &
        dtF_KG(const FieldState &stateIn, FieldState &stateOut, Real t) {
            const auto &ϕᵢₙ  = stateIn .getPhi();
            const auto &δₜϕᵢₙ = stateIn .getDPhiDt();
            auto       &ϕₒᵤₜ  = stateOut.getPhi();
            auto       &δₜϕₒᵤₜ = stateOut.getDPhiDt();

            auto &L = *O;
            GET Lϕ = *temp1;
            GET dVdϕ = *temp2;
            Lϕ = L*ϕᵢₙ;
            ϕᵢₙ .Apply(*dVDPhi, dVdϕ);

            auto &fₑₓₜ = *f;



            /* EQ 1 */
            ϕₒᵤₜ = δₜϕᵢₙ;

            /* EQ 2 */
            δₜϕₒᵤₜ = Lϕ - dVdϕ;
            if(&fₑₓₜ != nullptr) δₜϕₒᵤₜ += fₑₓₜ;

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
