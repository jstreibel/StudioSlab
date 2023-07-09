//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_FIELDSTATE_H
#define V_SHAPE_FIELDSTATE_H

#include <Phys/Space/Impl/ArithmeticOperationInterface.h>
#include <Phys/Space/Impl/DiscreteSpace.h>

#include <Phys/Numerics/Output/Util/FieldStateOutputInterface.h>

namespace Base {

    /**
     * State representing 2nd order equation state.
     * @tparam EqCategory Usually some DiscreteFunction type.
     */
    template<class EqCategory>
    class EquationState : public Utils::ArithmeticOpsInterface<EquationState<EqCategory>>, public FStateOutputInterface {
    public:
        EquationState(EqCategory *phi, EqCategory *dPhiDt) : phi(phi), dPhiDt(dPhiDt) {}

        using FunctionType = typename EqCategory::Type;
        using FunctionArbitraryType = EqCategory;

        virtual ~EquationState() {
            delete phi;
            delete dPhiDt;
        }

        EquationState &Add(const EquationState &fieldState) override {
            phi->Add(fieldState.getPhi());
            dPhiDt->Add(fieldState.getDPhiDt());

            return *this;
        }
        EquationState &Subtract(const EquationState<EqCategory> &fieldState) override {
            phi->Subtract(fieldState.getPhi());
            dPhiDt->Subtract(fieldState.getDPhiDt());

            return *this;
        }
        EquationState &StoreAddition(const EquationState &fieldState1, const EquationState &fieldState2) override {
            phi->StoreAddition(fieldState1.getPhi(), fieldState2.getPhi());
            dPhiDt->StoreAddition(fieldState1.getDPhiDt(), fieldState2.getDPhiDt());

            return *this;
        }
        EquationState &Multiply(floatt a) override {
            phi->Multiply(a);
            dPhiDt->Multiply(a);

            return *this;
        }
        EquationState<EqCategory> &
                StoreSubtraction(const EquationState &aoi1, const EquationState &aoi2) override {
            phi->StoreSubtraction(aoi1.getPhi(), aoi2.getPhi());
            dPhiDt->StoreSubtraction(aoi1.getDPhiDt(), aoi2.getDPhiDt());

            return *this;
        }
        EquationState<EqCategory> &
                StoreMultiplication(const EquationState<EqCategory> &aoi1, const Real a) override {
            phi->StoreMultiplication(aoi1.getPhi(), a);
            dPhiDt->StoreMultiplication(aoi1.getDPhiDt(), a);

            return *this;
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

            const PosInt N = space.getTotalDiscreteSites();
            const VecFloat &vec = space.getHostData(true);

            for(PosInt n=0; n<N; n++)
                out << vec[n] << separator;
        }

        void outputdPhiDt(OStream &out, Str separator) const override {
            DiscreteSpace &space = dPhiDt->getSpace();
            const PosInt N = space.getTotalDiscreteSites();
            const VecFloat &vec = space.getHostData(true);

            for(PosInt n=0; n<N; n++)
                out << vec[n] << separator;
        }

    protected:
        EqCategory *phi;
        EqCategory *dPhiDt;
    };
}

#endif //V_SHAPE_FIELDSTATE_H
