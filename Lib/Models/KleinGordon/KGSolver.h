#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Math/DifferentialEquations/Solver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/SignumFunction.h"
#include "KGState.h"


namespace Slab::Models {

    using namespace Math;

    template<class STATE_T>
    requires DerivedFromKGState<STATE_T, typename STATE_T::CategoryType>
    class KGSolver : public Base::Solver {

    public:
        using Potential = RtoR::Function;
        typedef STATE_T::CategoryType DiscrFuncType;
        typedef STATE_T::CategoryType::MyType DiscreteBase;
        using NonHomogenousFunc = typename STATE_T::CategoryType::MyBase;
        using NonHomogenousPtr = Pointer<NonHomogenousFunc>;
        using Operator = Math::Operator<DiscreteBase>;

        KGSolver(const NumericConfig &params,
               Base::BoundaryConditions_ptr du,
               const Pointer<Operator> & O,
               const Pointer<Potential>& potential,
               NonHomogenousPtr nonHomogenousFunc=Pointer<NonHomogenousFunc>())
        : Base::Solver(params, du)
        , O(O)
        , V(potential)
        , dVDPhi(potential->diff(0))
        , f(nonHomogenousFunc)
        {    }

        ~KGSolver() override = default;

        void startStep(const Base::EquationState &state, Real t, Real dt) final {
            Base::Solver::startStep(state, t, dt);

            IN kgState = dynamic_cast<const STATE_T&>(state);
            this->startStep_KG(kgState, t, dt);
        }

        Base::EquationState &
        dtF(const Base::EquationState &stateIn, Base::EquationState &stateOut, Real t) final {
            auto &kgStateIn  = dynamic_cast<const STATE_T&>(stateIn );
            auto &kgStateOut = dynamic_cast<      STATE_T&>(stateOut);

            return dynamic_cast<Base::EquationState&>(dtF_KG(kgStateIn, kgStateOut, t));
        }

    protected:

        virtual void
        startStep_KG(const STATE_T &state, Real t, Real dt) {
            Base::Solver::startStep(state, t, dt);

            if(temp1 == nullptr){
                assert(temp2 == nullptr);

                temp1 = DynamicPointerCast<DiscrFuncType>(state.getPhi().Clone());
                temp2 = DynamicPointerCast<DiscrFuncType>(state.getPhi().Clone());
            }
        }

        virtual STATE_T &
        dtF_KG(const STATE_T &stateIn, STATE_T &stateOut, Real t) {
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
        Pointer<DiscrFuncType> temp1 = nullptr
        , temp2 = nullptr;
        Pointer<Potential> V;
        Pointer<Potential> dVDPhi = nullptr;
        NonHomogenousPtr f = nullptr;
    };

}




#endif // HamiltonCPU_H
