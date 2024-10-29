//
// Created by joao on 6/1/24.
//

#ifndef STUDIOSLAB_ODEINTTOSLABSTATE_H
#define STUDIOSLAB_ODEINTTOSLABSTATE_H

#include "OdeintState.h"

namespace Slab::Math::Base {

    using namespace Odeint;

    class OdeintToSlabState : public OdeintState<Base::EquationState> {
        typedef Base::EquationState SlabState;

        std::shared_ptr <SlabState> slabState;

    public:
        void applyOperation1(OdeintOperation<1, SlabState>) override;

        void applyOperation2(const OdeintState &,
                             OdeintOperation<2, SlabState>) override;

        void applyOperation3(const OdeintState &,
                             const OdeintState &,
                             OdeintOperation<3, SlabState>) override;

        void applyOperation4(const OdeintState &,
                             const OdeintState &,
                             const OdeintState &,
                             OdeintOperation<4, SlabState>) override;

        void applyOperation5(const OdeintState &,
                             const OdeintState &,
                             const OdeintState &,
                             const OdeintState &,
                             OdeintOperation<5, SlabState>) override;

        void applyOperation6(const OdeintState &,
                             const OdeintState &,
                             const OdeintState &,
                             const OdeintState &,
                             const OdeintState &,
                             OdeintOperation<6, SlabState>) override;

    };

}

#endif //STUDIOSLAB_ODEINTTOSLABSTATE_H
