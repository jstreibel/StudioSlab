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
        dtF_KG(const STATE_T &stateIn, STATE_T &stateOut, Real t, Real dt) {
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

                oDPhi.StoreSubtraction(laplacian, dVdphi_out);

                if(f != nullptr) oDPhi += *f;

                oDPhi *= dt;
            }

            return stateOut;
        }

    public:
        using Potential = RtoR::Function;
        typedef STATE_T::CategoryType DiscrFuncType;
        using NonHomogenousFunc = typename STATE_T::CategoryType::MyBase;
        using NonHomogenousPtr = Pointer<NonHomogenousFunc>;

        KGSolver(const NumericConfig &params,
               Base::BoundaryConditions_ptr du,
               Pointer<Potential> potential,
               NonHomogenousPtr nonHomogenousFunc=Pointer<NonHomogenousFunc>())
        : Base::Solver(params, du)
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
        dtF(const Base::EquationState &stateIn, Base::EquationState &stateOut, Real t, Real dt) final {
            auto &kgStateIn  = dynamic_cast<const STATE_T&>(stateIn );
            auto &kgStateOut = dynamic_cast<      STATE_T&>(stateOut);

            return dynamic_cast<Base::EquationState&>(dtF_KG(kgStateIn, kgStateOut, t, dt));
        }

        static bool isDissipating() { return false; }

    protected:
        Pointer<DiscrFuncType> temp1 = nullptr
                             , temp2 = nullptr;
        Pointer<Potential> V;
        Pointer<Potential> dVDPhi = nullptr;
        NonHomogenousPtr f = nullptr;
    };

}




#endif // HamiltonCPU_H
