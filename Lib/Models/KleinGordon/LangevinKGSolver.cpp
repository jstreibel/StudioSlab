//
// Created by joao on 12/08/23.
//

#include "LangevinKGSolver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Utils/RandUtils.h"
#include "Utils/OMPUtils.h"
#include "Math/Function/RtoR/Model/Operators/RtoRLaplacian.h"


namespace Slab::Models::KGRtoR {

    Real ξ() {
        return RandUtils::GaussianNoise(.0, 1.0);
    }

    LangevinKGSolver::LangevinKGSolver(Base::BoundaryConditions_ptr du,
                                       const Pointer<Potential>& potential)
    : KGRtoRSolver(du, New<RtoR::RtoRLaplacian>(), potential){

    }

    void LangevinKGSolver::ComputeImpulses() {
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

    void LangevinKGSolver::startStep_KG(const FieldState &kgState, Real t, Real dt) {
        KGRtoRSolver::startStep_KG(kgState, t, dt);

        α = sqrt(2 * T * γ / dt);

        ComputeImpulses();
    }

    LangevinKGSolver::FieldState &
    LangevinKGSolver::dtF_KG(const FieldState &kgStateIn, FieldState &kgStateOut, Real t) {
        if (langevinImpulses == nullptr) {
            assert(scaledImpulses == nullptr);

            langevinImpulses = DynamicPointerCast <RtoR::NumericFunction> (kgStateIn.getPhi().Clone());
            scaledImpulses   = DynamicPointerCast <RtoR::NumericFunction> (kgStateIn.getPhi().Clone());

            scaledImpulses->Set(RtoR::NullFunction());
        }

#pragma omp barrier

        kgStateOut = KGSolver::dtF_KG(kgStateIn, kgStateOut, t);

#pragma omp barrier

        {
            IN pi = kgStateIn.getDPhiDt();
            OUT dissipation = *temp1;

            dissipation = pi * (-γ);

            (*scaledImpulses) = *langevinImpulses * α;

            /*
            OUT F = kgStateOut.getDPhiDt();
            OUT F_ext = *temp2;
            F_ext.StoreAddition(*scaledImpulses, dissipation) *= dt;

            F.Add(F_ext);
             */

            OUT F = kgStateOut.getDPhiDt();
            OUT F_ext = *temp2;
            F_ext = *scaledImpulses + dissipation;

            F += F_ext;
        }

        return kgStateOut;
    }

    void LangevinKGSolver::setTemperature(Real value) { T = value; }

    void LangevinKGSolver::setDissipationCoefficient(Real value) { γ = value; }



}