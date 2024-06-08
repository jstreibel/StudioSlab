//
// Created by joao on 6/1/24.
//

#include "OdeintToSlabState.h"


namespace Slab::Math::Base {
    void OdeintToSlabState::applyOperation1(OdeintOperation<1, SlabState> operation) {
        operation(*slabState);
    }

    void OdeintToSlabState::applyOperation2(const OdeintState &state2,
                                            OdeintOperation<2, SlabState> operation) {
        const auto& slabState2 = *dynamic_cast<const OdeintToSlabState&>(state2).slabState;

        operation(*slabState, slabState2);
    }

    void OdeintToSlabState::applyOperation3(const OdeintState &state2,
                                            const OdeintState &state3,
                                            OdeintOperation<3, SlabState> operation) {
        const auto& slabState2 = *dynamic_cast<const OdeintToSlabState&>(state2).slabState;
        const auto& slabState3 = *dynamic_cast<const OdeintToSlabState&>(state3).slabState;

        operation(*slabState, slabState2, slabState3);
    }

    void OdeintToSlabState::applyOperation4(const OdeintState &state2,
                                            const OdeintState &state3,
                                            const OdeintState &state4,
                                            OdeintOperation<4, SlabState> operation) {
        const auto& slabState2 = *dynamic_cast<const OdeintToSlabState&>(state2).slabState;
        const auto& slabState3 = *dynamic_cast<const OdeintToSlabState&>(state3).slabState;
        const auto& slabState4 = *dynamic_cast<const OdeintToSlabState&>(state4).slabState;

        operation(*slabState, slabState2, slabState3, slabState4);
    }

    void OdeintToSlabState::applyOperation5(const OdeintState &state2,
                                            const OdeintState &state3,
                                            const OdeintState &state4,
                                            const OdeintState &state5,
                                            OdeintOperation<5, SlabState> operation) {
        const auto& slabState2 = *dynamic_cast<const OdeintToSlabState&>(state2).slabState;
        const auto& slabState3 = *dynamic_cast<const OdeintToSlabState&>(state3).slabState;
        const auto& slabState4 = *dynamic_cast<const OdeintToSlabState&>(state4).slabState;
        const auto& slabState5 = *dynamic_cast<const OdeintToSlabState&>(state5).slabState;

        operation(*slabState, slabState2, slabState3, slabState4, slabState5);
    }

    void OdeintToSlabState::applyOperation6(const OdeintState &state2,
                                            const OdeintState &state3,
                                            const OdeintState &state4,
                                            const OdeintState &state5,
                                            const OdeintState &state6,
                                            OdeintOperation<6, SlabState> operation) {
        const auto& slabState2 = *dynamic_cast<const OdeintToSlabState&>(state2).slabState;
        const auto& slabState3 = *dynamic_cast<const OdeintToSlabState&>(state3).slabState;
        const auto& slabState4 = *dynamic_cast<const OdeintToSlabState&>(state4).slabState;
        const auto& slabState5 = *dynamic_cast<const OdeintToSlabState&>(state5).slabState;
        const auto& slabState6 = *dynamic_cast<const OdeintToSlabState&>(state6).slabState;

        operation(*slabState, slabState2, slabState3, slabState4, slabState5, slabState6);
    }
}
