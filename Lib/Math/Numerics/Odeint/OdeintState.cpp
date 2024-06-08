//
// Created by joao on 6/1/24.
//

#include "OdeintState.h"

namespace Slab::Math::Odeint {

    OdeintStateWrapper::OdeintStateWrapper(const std::shared_ptr<OdeintState> &state) : odeintState(state) {}

    void OdeintStateWrapper::applyOperation1(OdeintOperation<1, Real> operation) {
        odeintState->applyOperation1(operation);
    }

    void OdeintStateWrapper::applyOperation2(const OdeintState &state2,
                                             OdeintOperation<2, Real> operation) {
        odeintState->applyOperation2(state2, operation);
    }

    void OdeintStateWrapper::applyOperation3(const OdeintState &state2,
                                             const OdeintState &state3,
                                             OdeintOperation<3, Real> operation) {
        odeintState->applyOperation3(state2, state3, operation);
    }

    void OdeintStateWrapper::applyOperation4(const OdeintState &state2,
                                             const OdeintState &state3,
                                             const OdeintState &state4,
                                             OdeintOperation<4, Real> operation) {
        odeintState->applyOperation4(state2, state3, state4, operation);
    }

    void OdeintStateWrapper::applyOperation5(const OdeintState &state2,
                                             const OdeintState &state3,
                                             const OdeintState &state4,
                                             const OdeintState &state5,
                                             OdeintOperation<5, Real> operation) {
        odeintState->applyOperation5(state2, state3, state4, state5, operation);
    }

    void OdeintStateWrapper::applyOperation6(const OdeintState &state2,
                                             const OdeintState &state3,
                                             const OdeintState &state4,
                                             const OdeintState &state5,
                                             const OdeintState &state6,
                                             OdeintOperation<6, Real> operation) {
        odeintState->applyOperation6(state2, state3, state4, state5, state6, operation);
    }

} // Slab::Math::Odeint