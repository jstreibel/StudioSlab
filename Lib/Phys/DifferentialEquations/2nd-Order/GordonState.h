//
// Created by joao on 7/21/23.
//

#ifndef STUDIOSLAB_GORDONSTATE_H
#define STUDIOSLAB_GORDONSTATE_H

#include "GordonStateT.h"
#include "../EquationState.h"

namespace Phys::Gordon {
    using SubState = Simulation::EquationState;
    using EqState  = Simulation::EquationState;

    class GordonState : public EqState {
    protected:
        SubState &u, &du;

    public:
        GordonState(const EqState &prototype) : u(*prototype.clone()), du(*prototype.clone()) {}

        EqState &Add(const EqState &rhs) override {
            auto &state = dynamic_cast<const GordonState&>(rhs);

            u .Add(state.u);
            du.Add(state.du);

            return *this;
        }

        EqState &Subtract(const EqState &rhs) override {
            auto &state = dynamic_cast<const GordonState&>(rhs);

            u .Subtract(state.u);
            du.Subtract(state.du);

            return *this;
        }

        EqState &StoreAddition(const EqState &lhs, const EqState &rhs) override {
            auto &state1 = dynamic_cast<const GordonState&>(lhs);
            auto &state2 = dynamic_cast<const GordonState&>(rhs);

            u .StoreAddition(state1. u, state2. u);
            du.StoreAddition(state1.du, state2.du);

            return *this;
        }

        EqState &StoreSubtraction(const EqState &lhs, const EqState &rhs) override {
            auto &state1 = dynamic_cast<const GordonState&>(lhs);
            auto &state2 = dynamic_cast<const GordonState&>(rhs);

            u .StoreSubtraction(state1. u, state2. u);
            du.StoreSubtraction(state1.du, state2.du);

            return *this;
        }

        EqState &StoreMultiplication(const EqState &lhs, const Real a) override {
            auto &state = dynamic_cast<const GordonState &>(lhs);

            u .StoreMultiplication(state. u, a);
            du.StoreMultiplication(state.du, a);

            return *this;
        }

        EqState &Multiply(Real a) override {
            u.Multiply(a);
            du.Multiply(a);

            return *this;
        }

        EqState &operator*=(Real a) override {
            u *= a;
            du *= a;

            return *this;
        }


    };
}

#endif //STUDIOSLAB_GORDONSTATE_H
