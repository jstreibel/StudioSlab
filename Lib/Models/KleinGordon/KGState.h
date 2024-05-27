//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_FIELDSTATE_H
#define V_SHAPE_FIELDSTATE_H

#include "Math/Space/Impl/DiscreteSpace.h"
#include "Math/DifferentialEquations/EquationState.h"

#include "Math/Numerics/Output/Util/FieldStateOutputInterface.h"

namespace Slab::Models {

    using namespace Slab::Math;

    /**
     * State representing 2nd order equation state.
     * @tparam EqCategory Usually some DiscreteFunction type.
     */
    template<class EqCategory>
    class KGState : public Base::EquationState, public EqStateOutputInterface {
        typedef Base::EquationState State;

    public:
        typedef EqCategory SubStateType;
        using FunctionType = typename EqCategory::Type;
        using FunctionArbitraryType = EqCategory;

        KGState(Pointer<EqCategory> phi, Pointer<EqCategory> dPhiDt)
        : phi(phi), dPhiDt(dPhiDt) {}
        KGState(const KGState& state)
        : KGState(new EqCategory(state.phi), new EqCategory(state.dPhiDt)) {};
        virtual ~KGState() = default;

        State &Add(const State &fieldState) override {
            auto &kgState = dynamic_cast<const KGState&>(fieldState);

            phi->Add(kgState.getPhi());
            dPhiDt->Add(kgState.getDPhiDt());

            return *this;
        }
        State &Subtract(const State &fieldState) override {
            auto &kgState = dynamic_cast<const KGState&>(fieldState);

            phi->Subtract(kgState.getPhi());
            dPhiDt->Subtract(kgState.getDPhiDt());

            return *this;
        }
        State &StoreAddition(const State &fieldState1, const State &fieldState2) override {
            auto &kgState1 = dynamic_cast<const KGState&>(fieldState1);
            auto &kgState2 = dynamic_cast<const KGState&>(fieldState2);

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
        State &StoreMultiplication(const State &state, const Real a) override {
            auto &kgState = dynamic_cast<const KGState&>(state);
            phi->StoreMultiplication(kgState.getPhi(), a);
            dPhiDt->StoreMultiplication(kgState.getDPhiDt(), a);

            return *this;
        }

        void set(const State &state) override {
            auto &kgState = dynamic_cast<const KGState&>(state);

            setPhi(kgState.getPhi());
            setDPhiDt(kgState.getDPhiDt());
        }

        /*! Basicamente utilizado por BoundaryConditions */
        void setPhi(const typename EqCategory::Type &function) {
            phi->Set(function);
        }
        /*! Basicamente utilizado por BoundaryConditions */
        void setDPhiDt(const typename EqCategory::Type &function) {
            dPhiDt->Set(function);
        }

        EqCategory &getPhi() { return *phi; }
        EqCategory &getPhi() const { return *phi; }
        EqCategory &getDPhiDt() { return *dPhiDt; }
        EqCategory &getDPhiDt() const { return *dPhiDt; }

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
        Pointer<EqCategory> phi;
        Pointer<EqCategory> dPhiDt;
    };
}

#endif //V_SHAPE_FIELDSTATE_H
