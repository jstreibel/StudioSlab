//
// Created by joao on 10/28/24.
//

#ifndef STUDIOSLAB_MOLECULESSTATE_H
#define STUDIOSLAB_MOLECULESSTATE_H

#include "Math/Numerics/ODE/Solver/EquationState.h"
#include "Graphics/Types2D.h"

#include "OpDefs.h"

namespace Slab::Models::MolecularDynamics {

    // Used for Verlet
    // typedef Pair<Graphics::PointContainer &, Graphics::PointContainer &> State;
    typedef Pair<Graphics::PointContainer, Graphics::PointContainer> VerletMoleculeContainer;

    class MoleculesState : public Math::Base::EquationState {
        Pair<Graphics::PointContainer &, Graphics::PointContainer &> data;

        static auto& Cast(const auto &eqState)
        { return dynamic_cast<const MoleculesState&>(eqState); }

        static auto& Cast(auto &eqState)
        { return dynamic_cast<MoleculesState&>(eqState); }


    public:
        MoleculesState(Graphics::PointContainer &q, Graphics::PointContainer &p) : data(q, p) {};

        [[nodiscard]] Graphics::PointContainer &first() const { return data.first; }
        [[nodiscard]] Graphics::PointContainer &second() const { return data.second; }

        [[nodiscard]] auto category() const -> Str override;

        auto replicate() const -> Pointer<EquationState> override {
            NOT_IMPLEMENTED_CLASS_METHOD
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

        EquationState &StoreScalarMultiplication(const EquationState &eqState, DevFloat a) override {
            StoreScalarOperation(*)
            return *this;
        }

        EquationState &Multiply(DevFloat a) override {
            ScalarOperation(*=)
            return *this;
        }

        EquationState &StoreAddition(const EquationState &eqState1, const EquationState &eqState2) override {
            StoreOperation(+)

            return *this;
        }

        EquationState &StoreSubtraction(const EquationState &eqState1, const EquationState &eqState2) override {
            StoreOperation(-)

            return *this;
        }
    };

} // Slab::Models::MolecularDynamics

#endif //STUDIOSLAB_MOLECULESSTATE_H
