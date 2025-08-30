//
// Created by joao on 4/11/23.
//

#ifndef STUDIOSLAB_LEADINGDELTA_H
#define STUDIOSLAB_LEADINGDELTA_H



#include "Models/KleinGordon/R2toR/R2toRBuilder.h"
#include "Math/Numerics/ODE/Solver/BoundaryConditions.h"
#include "Models/KleinGordon/R2toR/BoundaryConditions/R2ToRBoundaryCondition.h"

#include "Monitor.h"
#include "RingDeltaFunc.h"

namespace Studios::Fields::R2toRLeadingDelta {

    using namespace Slab;
    using namespace Math;

    extern TPointer<RingDeltaFunc> ringDelta1;

    class BoundaryCondition : public Slab::Math::Base::BoundaryConditions {
        TPointer<RingDeltaFunc> ringDelta;
        DevFloat tf;
        bool deltaSpeedOp;

    public:
        explicit BoundaryCondition(const TPointer<const R2toR::EquationState>& prototype,
                                   TPointer<RingDeltaFunc> ringDelta,
                                   DevFloat tf, bool deltaOperatesOnSpeed);
        void Apply(Slab::Math::Base::EquationState &function, DevFloat t) const override;
    };


class Builder : public Models::KGR2toR::Builder {
        RealParameter      W_0              = RealParameter(0.1, FParameterDescription{'W', "W_0", "The height of corresponding analytic shockwave, which determines scale of the delta as C_n=(n-1)/2 W(0) with n=2."});
        RealParameter      eps              = RealParameter(0.1, FParameterDescription{"eps", "Half the base width of regularized delta;"});
        RealParameter      deltaDuration    = RealParameter(-1,  FParameterDescription{"delta_duration", "The duration of regularized delta. Negative values mean forever;"});

        TPointer<RingDeltaFunc> drivingFunc;

    protected:
        auto buildOpenGLOutput() -> Models::KGR2toR::OutputOpenGL * override;

    public:
        void *getHamiltonian() override;

    public:
        explicit Builder(bool do_register = false);

        auto NotifyInterfaceSetupIsFinished() -> void   override;

        auto getBoundary()               -> Slab::Math::Base::BoundaryConditions_ptr override;

        auto SuggestFileName() const -> Str override;


    };

}


#endif //STUDIOSLAB_LEADINGDELTA_H
