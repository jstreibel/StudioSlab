//
// Created by joao on 9/08/23.
//

#ifndef STUDIOSLAB_PARTICLE_H
#define STUDIOSLAB_PARTICLE_H

#include "Utils/Types.h"

#include "Math/Numerics/Solver/EquationState.h"
#include "Graphics/Types2D.h"

#define CUTOFF_RADIUS 4


namespace MolecularDynamics {
    using namespace Slab;

#define AssertSizes(a) assert((a).first().size() == (a).second().size());
#define AssertCompatibility(a, b)    \
    assert((a).data.first.size() == (b).data.first.size());

#define TinySetup(STATE)                \
    auto &state = Cast(STATE);          \
    AssertCompatibility(*this, state);

#define Setup(STATE, TYPE)              \
    AssertSizes(STATE)                  \
    TYPE  q_##TYPE = (STATE).first();   \
    TYPE  p_##TYPE = (STATE).second();

#define Operation(OP)                   \
    {                                   \
        TinySetup(eqState)              \
        Setup(state, IN)                \
        Setup(*this, OUT)               \
                                        \
        fix N = first().size();         \
        for(int i=0; i<N; ++i){         \
            q_OUT[i] OP q_IN[i];        \
            p_OUT[i] OP p_IN[i];        \
        }                               \
    }

#define StoreOperation(OP)              \
{                                       \
        TinySetup(eqState)              \
        Setup(state, IN)                \
        Setup(*this, OUT)               \
                                        \
        fix N = first().size();         \
        for(int i=0; i<N; ++i){         \
            q_OUT[i] OP q_IN[i];        \
            p_OUT[i] OP p_IN[i];        \
        }                               \
    }

#define ScalarOperation(OP)             \
    {                                   \
        Setup(*this, OUT)               \
                                        \
        fix N = first().size();         \
        for(int i=0; i<N; ++i){         \
            q_OUT[i] OP a;              \
            p_OUT[i] OP a;              \
        }                               \
    }

#define StoreScalarOperation(OP)        \
    {                                   \
        TinySetup(eqState)              \
        Setup(state, IN)                \
        Setup(*this, OUT)               \
                                        \
        fix N = first().size();         \
        for(int i=0; i<N; ++i){         \
            q_OUT[i] = a OP q_IN[i];    \
            p_OUT[i] = a OP p_IN[i];    \
        }                               \
    }

    // Used for Verlet
    // typedef Pair<Graphics::PointContainer &, Graphics::PointContainer &> State;
    typedef Pair<Graphics::PointContainer, Graphics::PointContainer> VerletMoleculeContainer;

    class State : public Math::Base::EquationState {
        Pair<Graphics::PointContainer &, Graphics::PointContainer &> data;

        static auto& Cast(const auto &eqState)
        { return dynamic_cast<const State&>(eqState); }

        static auto& Cast(auto &eqState)
        { return dynamic_cast<State&>(eqState); }


    public:
        State(Graphics::PointContainer &q, Graphics::PointContainer &p) : data(q, p) {};

        Graphics::PointContainer &first() const { return data.first; }
        Graphics::PointContainer &second() const { return data.second; }

        auto replicate() const -> Pointer<EquationState> override {

        }

        auto setData (const EquationState &eqState) -> void            override Operation(=)
        auto Add     (const EquationState &eqState) -> EquationState & override {
            Operation(+=)
            return *this;
        }
        auto Subtract(const EquationState &eqState) -> EquationState & override {
            Operation(-=)
            return *this;
        }

        EquationState &StoreScalarMultiplication(const EquationState &eqState, Real a) override {
            StoreScalarOperation(*)
            return *this;
        }

        EquationState &Multiply(Real a) override {
            ScalarOperation(*=)
            return *this;
        }

        EquationState &StoreAddition(const EquationState &eqState1, const EquationState &eqState2) override {

        }

        EquationState &StoreSubtraction(const EquationState &eqState1, const EquationState &eqState2) override {
            NOT_IMPLEMENTED_CLASS_METHOD
        }
    };


// Used for RK4
    struct Molecule {
        Molecule(Graphics::Point2D q, Graphics::Point2D p) : m_q(q), m_p(p) {}

        Molecule() = default;

        Graphics::Point2D m_q, m_p;
    };

    Molecule operator*(const Real &a, const Molecule &m);

    Molecule operator+(const Molecule &a, const Molecule &b);

    typedef Vector<Molecule> MoleculeContainer;

}

#endif //STUDIOSLAB_PARTICLE_H
