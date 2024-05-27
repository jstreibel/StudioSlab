#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Math/DifferentialEquations/EquationSolver.h"

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/SignumFunction.h"


#define CLONE(func) dynamic_cast<DiscrFuncType&>(*(func.Clone()))



namespace Slab::Models {

    using namespace Math;

    template<class State>
    class Solver : public Base::EquationSolverT<State> {
    public:
        using MyBase = Base::EquationSolverT<State>;
        using PotentialFunc = Base::FunctionT<Real, Real>;
        typedef State::SubStateType           DiscrFuncType;
        using NonHomogenousFunc = typename State::SubStateType::MyBase;
        using NonHomogenousPtr = Pointer<NonHomogenousFunc>;

        Solver(const NumericConfig &params,
               MyBase::EqBoundaryCondition_ptr du,
               PotentialFunc &potential,
               NonHomogenousPtr nonHomogenousFunc= Pointer<NonHomogenousFunc>())
        : Base::EquationSolverT<State>(params, du)
        , V(potential)
        , dVDPhi(potential.diff(0))
        , f(nonHomogenousFunc)
        {    }

        ~Solver() override {
            delete temp1;
            delete temp2;
            delete &V;
        }

        void startStep(const State &in, Real t, Real dt) override {
            MyBase::startStep(in, t, dt);

            if(temp1 == nullptr){
                assert(temp2 == nullptr);

                temp1 = (DiscrFuncType*)in.getPhi().Clone();
                temp2    = (DiscrFuncType*)in.getPhi().Clone();
            }
        }

        State &
        dtF(const State &stateIn, State &stateOut, Real t, Real dt) override {
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

        static bool isDissipating() { return false; }

    protected:
        DiscrFuncType *temp1 = nullptr
                    , *temp2 = nullptr;
        PotentialFunc &V;
        PotentialFunc::Ptr dVDPhi= nullptr;
        NonHomogenousPtr f = nullptr;
    };

}




#endif // HamiltonCPU_H
