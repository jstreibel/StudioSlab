//
// Created by joao on 6/1/24.
//

#ifndef STUDIOSLAB_ODEINTSTATE_H
#define STUDIOSLAB_ODEINTSTATE_H


#include <functional>
#include <memory>

#include "OdeintSignatureHelper.h"
#include "Math/Numerics/ODE/Solver/EquationState.h"

namespace Slab::Math::Odeint {

    template<typename T>
    class OdeintState {
        using Type = T;
    public:

        virtual void applyOperation1(OdeintOperation<1, T>) = 0;
        virtual void applyOperation2(const OdeintState&,
                                     OdeintOperation<2, T>) = 0;
        virtual void applyOperation3(const OdeintState&,
                                     const OdeintState&,
                                     OdeintOperation<3, T>) = 0;
        virtual void applyOperation4(const OdeintState&,
                                     const OdeintState&,
                                     const OdeintState&,
                                     OdeintOperation<4, T>) = 0;
        virtual void applyOperation5(const OdeintState&,
                                     const OdeintState&,
                                     const OdeintState&,
                                     const OdeintState&,
                                     OdeintOperation<5, T>) = 0;
        virtual void applyOperation6(const OdeintState&,
                                     const OdeintState&,
                                     const OdeintState&,
                                     const OdeintState&,
                                     const OdeintState&,
                                     OdeintOperation<6, T>) = 0;
    };

    class OdeintStateWrapper : public OdeintState<DevFloat> {
        std::shared_ptr<OdeintState> odeintState = nullptr;

    public:
        OdeintStateWrapper() = default;

        explicit OdeintStateWrapper(const std::shared_ptr<OdeintState> &state);

        void applyOperation1(OdeintOperation<1, DevFloat> operation) override;

        void applyOperation2(const OdeintState &state2,
                             OdeintOperation<2, DevFloat> operation) override;

        void applyOperation3(const OdeintState &state2,
                             const OdeintState &state3,
                             OdeintOperation<3, DevFloat> operation) override;

        void applyOperation4(const OdeintState &state2,
                             const OdeintState &state3,
                             const OdeintState &state4,
                             OdeintOperation<4, DevFloat> operation) override;

        void applyOperation5(const OdeintState &state2,
                             const OdeintState &state3,
                             const OdeintState &state4,
                             const OdeintState &state5,
                             OdeintOperation<5, DevFloat> operation) override;

        void applyOperation6(const OdeintState &state2,
                             const OdeintState &state3,
                             const OdeintState &state4,
                             const OdeintState &state5,
                             const OdeintState &state6,
                             OdeintOperation<6, DevFloat> operation) override;
    };


} // Slab::Math::Odeint


#endif //STUDIOSLAB_ODEINTSTATE_H
