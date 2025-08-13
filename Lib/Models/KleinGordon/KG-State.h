//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_FIELDSTATE_H

#define V_SHAPE_FIELDSTATE_H

#include "Math/VectorSpace/Impl/DiscreteSpace.h"
#include "Math/Function/NumericFunction.h"
#include "Math/Numerics/ODE/Solver/EquationState.h"

#include "Math/Numerics/ODE/Output/Util/FieldStateOutputInterface.h"

namespace Slab::Models {

    using namespace Slab::Math;

    /**
     * State representing 2nd order equation state.
     * @tparam EqCategory Usually some NumericFunction type.
     */
    template<class InCategory>
    class KGState : public Base::EquationState, public EqStateOutputInterface {
    public:
        typedef Base::EquationState State;
        typedef DevFloat OutCategory;
        typedef Base::NumericFunction<InCategory, OutCategory> Field;
        typedef Base::FunctionT<InCategory, OutCategory> FieldBase;
        typedef TPointer<Field> Field_ptr;

        KGState(TPointer<Field> phi, TPointer<Field> dPhiDt)
        : Phi(phi), dPhiDt(dPhiDt) {}

        KGState(const KGState& state)
        : KGState(state.Phi, state.dPhiDt) {};

        ~KGState() override = default;

        State &Add(const State &state) override {
            auto &kgState = dynamic_cast<const KGState&>(state);

            Phi   ->Add(kgState.getPhi());
            dPhiDt->Add(kgState.getDPhiDt());

            return *this;
        }
        State &Subtract(const State &state) override {
            auto &kgState = dynamic_cast<const KGState&>(state);

            Phi   ->Subtract(kgState.getPhi());
            dPhiDt->Subtract(kgState.getDPhiDt());

            return *this;
        }
        State &StoreAddition(const State &state1, const State &state2) override {
            auto &kgState1 = dynamic_cast<const KGState&>(state1);
            auto &kgState2 = dynamic_cast<const KGState&>(state2);

            Phi->StoreAddition(kgState1.getPhi(), kgState2.getPhi());
            dPhiDt->StoreAddition(kgState1.getDPhiDt(), kgState2.getDPhiDt());

            return *this;
        }
        State &Multiply(floatt a) override {
            Phi->Multiply(a);
            dPhiDt->Multiply(a);

            return *this;
        }
        State &StoreSubtraction(const State &state1, const State &state2) override {
            auto &kgState1 = dynamic_cast<const KGState&>(state1);
            auto &kgState2 = dynamic_cast<const KGState&>(state2);

            Phi->StoreSubtraction(kgState1.getPhi(), kgState2.getPhi());
            dPhiDt->StoreSubtraction(kgState1.getDPhiDt(), kgState2.getDPhiDt());

            return *this;
        }
        State &StoreScalarMultiplication(const State &state, const DevFloat a) override {
            auto &kgState = dynamic_cast<const KGState&>(state);
            Phi->StoreScalarMultiplication(kgState.getPhi(), a);
            dPhiDt->StoreScalarMultiplication(kgState.getDPhiDt(), a);

            return *this;
        }

        void setData(const State &state) override {
            auto &kgState = dynamic_cast<const KGState&>(state);

            SetPhi(kgState.getPhi());
            SetDPhiDt(kgState.getDPhiDt());
        }

        /*! Basicamente utilizado por BoundaryConditions */
        void SetPhi(const FieldBase &function) {
            Phi->Set(function);
        }
        /*! Basicamente utilizado por BoundaryConditions */
        void SetDPhiDt(const FieldBase &function) {
            dPhiDt->Set(function);
        }

        Field &getPhi() { return *Phi; }
        Field &getPhi() const { return *Phi; }
        Field &getDPhiDt() { return *dPhiDt; }
        Field &getDPhiDt() const { return *dPhiDt; }

        void outputPhi(OStream &out, Str separator) const override {
            DiscreteSpace &space = Phi->getSpace();

            const UInt N = space.getTotalDiscreteSites();
            const RealArray &vec = space.getHostData(true);

            for(UInt n=0; n<N; n++)
                out << vec[n] << separator;
        }

        void outputdPhiDt(OStream &out, Str separator) const override {
            DiscreteSpace &space = dPhiDt->getSpace();
            const UInt N = space.getTotalDiscreteSites();
            const RealArray &vec = space.getHostData(true);

            for(UInt n=0; n<N; n++)
                out << vec[n] << separator;
        }

    protected:
        Field_ptr Phi;
        Field_ptr dPhiDt;
    };

}

#endif //V_SHAPE_FIELDSTATE_H
