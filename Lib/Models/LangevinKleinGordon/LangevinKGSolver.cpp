//
// Created by joao on 12/08/23.
//

#include "LangevinKGSolver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Utils/RandUtils.h"
#include "Utils/OMPUtils.h"


namespace Slab::Math::RtoR {

    Real ξ() {
        return RandUtils::gaussianNoise(.0, 1.0);
    }

    void RtoR::LangevinKGSolver::ComputeImpulses() {
        if (langevinImpulses == nullptr) return;

        auto &space = langevinImpulses->getSpace();
        assert(space.getMetaData().getNDim() == 1);

        auto &X = space.getHostData();

        fix nSplit = 1;
        OMP_PARALLEL_FOR(i, X.size() / nSplit) {
            fix x = ξ();
            fix k = nSplit * i;
            for (auto n = 0; n < nSplit; ++n) {
                auto &x1 = X[k + n];
                x1 = x;
            }
        }

        space.upload();
    }

    void RtoR::LangevinKGSolver::startStep(const EqState &stateIn, Real t, Real dt) {
        RtoR::KGSolver::startStep(stateIn, t, dt);

        ComputeImpulses();
    }

    RtoR::EquationState &
    RtoR::LangevinKGSolver::dtF(const RtoR::EquationState &stateIn, RtoR::EquationState &stateOut, Real t, Real dt) {
        if (langevinImpulses == nullptr) {
            assert(scaledImpulses == nullptr);

            langevinImpulses = (EqState::SubStateType *) stateIn.getPhi().Clone();
            scaledImpulses = (EqState::SubStateType *) stateIn.getPhi().Clone();

            scaledImpulses->Set(RtoR::NullFunction());
        }

#pragma omp barrier

        stateOut = RtoR::KGSolver::dtF(stateIn, stateOut, t, dt);

#pragma omp barrier

        {
            IN pi = stateIn.getDPhiDt();
            OUT dissipation = *temp1;
            dissipation.StoreMultiplication(pi, -γ);

            fix h = params.geth();
            fix alpha = sqrt(2 * T * γ / dt);
            scaledImpulses->StoreMultiplication(*langevinImpulses, alpha);

            OUT F = stateOut.getDPhiDt();
            OUT F_ext = *temp2;
            F_ext.StoreAddition(*scaledImpulses, dissipation) *= dt;

            F.Add(F_ext);
        }

        return stateOut;
    }

    void RtoR::LangevinKGSolver::setTemperature(Real value) { T = value; }

    void RtoR::LangevinKGSolver::setDissipationCoefficient(Real value) { γ = value; }


}