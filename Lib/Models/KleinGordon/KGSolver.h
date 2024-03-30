#ifndef HamiltonCPU_H
#define HamiltonCPU_H

#include "Math/DifferentialEquations/EquationSolver.h"

#include "Math/Numerics/Builder.h"
#include "Maps/RtoR/Model/FunctionsCollection/SignumFunction.h"


#define CLONE(func) dynamic_cast<DiscrFuncType&>(*(func.Clone()))



namespace Fields::KleinGordon {

    template<class EqState>
    class Solver : public Slab::EquationSolverT<EqState> {
    public:
        using MyBase = Slab::EquationSolverT<EqState>;
        using PotentialFunc = Core::FunctionT<Real, Real>;
        typedef EqState::SubStateType           DiscrFuncType;
        using NonHomogenousFunc = typename EqState::SubStateType::MyBase;
        using NonHomogenousPtr = std::shared_ptr<NonHomogenousFunc>;

        Solver(const NumericConfig &params,
                        MyBase::EqBoundaryCondition &du,
                        PotentialFunc &potential,
                        NonHomogenousPtr nonHomogenousFunc= NullPtr<NonHomogenousFunc>())
        : Slab::EquationSolverT<EqState>(params, du)
        , V(potential)
        , dVDPhi(potential.diff(0))
        , f(nonHomogenousFunc)
        {    }

        ~Solver() override {
            delete temp1;
            delete temp2;
            delete &V;
        }

        void startStep(const EqState &in, Real t, Real dt) override {
            MyBase::startStep(in, t, dt);

            if(temp1 == nullptr){
                assert(temp2 == nullptr);

                temp1 = (DiscrFuncType*)in.getPhi().Clone();
                temp2    = (DiscrFuncType*)in.getPhi().Clone();
            }
        }

        EqState &
        dtF(const EqState &stateIn, EqState &stateOut, Real t, Real dt) override {
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
