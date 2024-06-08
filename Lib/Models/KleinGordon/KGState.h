//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_FIELDSTATE_H

#include "Utils/Templates.h"
#define V_SHAPE_FIELDSTATE_H

#include "Math/VectorSpace/Impl/DiscreteSpace.h"
#include "Math/Function/DiscreteFunction.h"
#include "Math/Numerics/Solver/EquationState.h"

#include "Math/Numerics/Output/Util/FieldStateOutputInterface.h"

namespace Slab::Models {

    using namespace Slab::Math;

    /**
     * State representing 2nd order equation state.
     * @tparam EqCategory Usually some DiscreteFunction type.
     */
    template<class InCategory>
    class KGState : public Base::EquationState, public EqStateOutputInterface {
    public:
        typedef Base::EquationState State;
        typedef Real OutCategory;
        typedef Base::DiscreteFunction<InCategory, OutCategory> Field;
        typedef Base::FunctionT<InCategory, OutCategory> FieldBase;
        typedef Pointer<Field> Field_ptr;

        KGState(Pointer<Field> phi, Pointer<Field> dPhiDt)
        : phi(phi), dPhiDt(dPhiDt) {}

        KGState(const KGState& state)
        : KGState(state.phi, state.dPhiDt) {};

        virtual ~KGState() = default;

        State &Add(const State &state) override {
            auto &kgState = dynamic_cast<const KGState&>(state);

            phi   ->Add(kgState.getPhi());
            dPhiDt->Add(kgState.getDPhiDt());

            return *this;
        }
        State &Subtract(const State &state) override {
            auto &kgState = dynamic_cast<const KGState&>(state);

            phi   ->Subtract(kgState.getPhi());
            dPhiDt->Subtract(kgState.getDPhiDt());

            return *this;
        }
        State &StoreAddition(const State &state1, const State &state2) override {
            auto &kgState1 = dynamic_cast<const KGState&>(state1);
            auto &kgState2 = dynamic_cast<const KGState&>(state2);

            phi->StoreAddition(kgState1.getPhi(), kgState2.getPhi());
            dPhiDt->StoreAddition(kgState1.getDPhiDt(), kgState2.getDPhiDt());

            return *this;
        }
        State &Multiply(floatt a) override {
            phi->Multiply(a);
            dPhiDt->Multiply(a);

            return *this;
        }
        State &StoreSubtraction(const State &state1, const State &state2) override {
            auto &kgState1 = dynamic_cast<const KGState&>(state1);
            auto &kgState2 = dynamic_cast<const KGState&>(state2);

            phi->StoreSubtraction(kgState1.getPhi(), kgState2.getPhi());
            dPhiDt->StoreSubtraction(kgState1.getDPhiDt(), kgState2.getDPhiDt());

            return *this;
        }
        State &StoreScalarMultiplication(const State &state, const Real a) override {
            auto &kgState = dynamic_cast<const KGState&>(state);
            phi->StoreScalarMultiplication(kgState.getPhi(), a);
            dPhiDt->StoreScalarMultiplication(kgState.getDPhiDt(), a);

            return *this;
        }

        void setData(const State &state) override {
            auto &kgState = dynamic_cast<const KGState&>(state);

            setPhi(kgState.getPhi());
            setDPhiDt(kgState.getDPhiDt());
        }

        /*! Basicamente utilizado por BoundaryConditions */
        void setPhi(const FieldBase &function) {
            phi->Set(function);
        }
        /*! Basicamente utilizado por BoundaryConditions */
        void setDPhiDt(const FieldBase &function) {
            dPhiDt->Set(function);
        }

        Field &getPhi() { return *phi; }
        Field &getPhi() const { return *phi; }
        Field &getDPhiDt() { return *dPhiDt; }
        Field &getDPhiDt() const { return *dPhiDt; }

        void outputPhi(OStream &out, Str separator) const override {
            DiscreteSpace &space = phi->getSpace();

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
        Field_ptr phi;
        Field_ptr dPhiDt;
    };

}

#endif //V_SHAPE_FIELDSTATE_H
