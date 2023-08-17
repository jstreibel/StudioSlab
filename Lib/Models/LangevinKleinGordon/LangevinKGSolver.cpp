//
// Created by joao on 12/08/23.
//

#include "LangevinKGSolver.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Common/RandUtils.h"
#include "Common/OMPUtils.h"

Real xi() {
    /*
    const Real z = FRANDOM;

    const Real r = sqrt(-2.0 * log(1.0 - z));
    // cheat?
    // const Real r = sqrt(-log(1.0 - z));

    static Real signs[] = {-1,1};
    const Real sign = signs[random()%2];

    return sign*r;
     */

    return RandUtils::gaussianNoise(.0, 1.0);
}

void RtoR::LangevinKGSolver::startStep(Real t, Real dt) {
    RtoR::KGSolver ::startStep(t, dt);

    ComputeImpulses();
}

void RtoR::LangevinKGSolver::ComputeImpulses() {
    if (langevinImpulses == nullptr) return;

    auto &space = langevinImpulses->getSpace();
    assert(space.getDim().getNDim() == 1);

    auto &X = space.getHostData();

    OMP_PARALLEL_FOR(i, X.size()) {
        auto &x = X[i];

        x = xi();
    }

    space.upload();
}

RtoR::EquationState &
RtoR::LangevinKGSolver::dtF(const RtoR::EquationState &stateIn, RtoR::EquationState &stateOut, Real t, Real dt) {
    if (langevinImpulses == nullptr) {
        assert(scaledImpulses == nullptr);

        langevinImpulses = (EqState::SubStateType *) stateIn.getPhi().Clone();
        scaledImpulses   = (EqState::SubStateType *) stateIn.getPhi().Clone();

        scaledImpulses->Set(RtoR::NullFunction());
    }

    #pragma omp barrier

    stateOut = RtoR::KGSolver::dtF(stateIn, stateOut, t, dt);

    #pragma omp barrier

    {
        IN pi  = stateIn.getDPhiDt();
        OUT dissipation = *temp1;
        dissipation.StoreMultiplication(pi, -γ);

        fix h = params.geth();
        fix alpha = sqrt(2 * T * γ  / dt);
        scaledImpulses->StoreMultiplication(*langevinImpulses, alpha);

        OUT F = stateOut.getDPhiDt();
        OUT F_ext = *temp2;
        F_ext.StoreAddition(*scaledImpulses, dissipation) *= dt;

        F.Add(F_ext);
    }

    return stateOut;
}

void RtoR::LangevinKGSolver::setTemperature(Real value) {T = value;}

void RtoR::LangevinKGSolver::setDissipationCoefficient(Real value) {γ = value;}

