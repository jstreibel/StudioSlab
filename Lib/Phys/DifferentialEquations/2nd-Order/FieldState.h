//
// Created by joao on 30/08/2019.
//

#ifndef V_SHAPE_FIELDSTATE_H
#define V_SHAPE_FIELDSTATE_H

#include <Phys/Space/ArithmeticOperationInterface.h>
#include <Phys/Space/DiscreteSpace.h>

#include <Phys/Numerics/Output/Util/FieldStateOutputInterface.h>

namespace Base {

    template<class ArbitraryFunctionType>
    class FieldState : public Utils::ArithmeticOpsInterface<FieldState<ArbitraryFunctionType>>, public FStateOutputInterface {
    public:
        FieldState(ArbitraryFunctionType *phi, ArbitraryFunctionType *dPhiDt) : phi(phi), dPhiDt(dPhiDt) {}

        using FunctionType = typename ArbitraryFunctionType::BaseType;
        using FunctionArbitraryType = ArbitraryFunctionType;

        ~FieldState() {
            delete phi;
            delete dPhiDt;
        }

        FieldState &Add(const FieldState &fieldState) override {
            phi->Add(fieldState.getPhi());
            dPhiDt->Add(fieldState.getDPhiDt());

            return *this;
        }
        FieldState &StoreAddition(const FieldState &fieldState1, const FieldState &fieldState2) override {
            phi->StoreAddition(fieldState1.getPhi(), fieldState2.getPhi());
            dPhiDt->StoreAddition(fieldState1.getDPhiDt(), fieldState2.getDPhiDt());

            return *this;
        }
        FieldState &Multiply(floatt a) override {
            phi->Multiply(a);
            dPhiDt->Multiply(a);

            return *this;
        }
        FieldState<ArbitraryFunctionType> &StoreSubtraction(const FieldState &aoi1,
                                                               const FieldState &aoi2) override {
            phi->StoreSubtraction(aoi1.getPhi(), aoi2.getPhi());
            dPhiDt->StoreSubtraction(aoi1.getDPhiDt(), aoi2.getDPhiDt());

            return *this;
        }

        /*! Basicamente utilizado por BoundaryConditions */
        void setPhi(const typename ArbitraryFunctionType::BaseType &function) {
            phi->Set(function);
        }
        /*! Basicamente utilizado por BoundaryConditions */
        void setDPhiDt(const typename ArbitraryFunctionType::BaseType &function) {
            dPhiDt->Set(function);
        }

        ArbitraryFunctionType &getPhi() { return *phi; }
        ArbitraryFunctionType &getPhi() const { return *phi; }
        ArbitraryFunctionType &getDPhiDt() { return *dPhiDt; }
        ArbitraryFunctionType &getDPhiDt() const { return *dPhiDt; }

        void outputPhi(std::ostream &out, String separator) const override {
            DiscreteSpace &space = phi->getSpace();

            const PosInt N = space.getTotalDiscreteSites();
            const VecFloat &vec = space.getHostData(true);

            for(PosInt n=0; n<N; n++)
                out << vec[n] << separator;
        }

        void outputdPhiDt(std::ostream &out, String separator) const override {
            DiscreteSpace &space = dPhiDt->getSpace();
            const PosInt N = space.getTotalDiscreteSites();
            const VecFloat &vec = space.getHostData(true);

            for(PosInt n=0; n<N; n++)
                out << vec[n] << separator;
        }

    protected:
        ArbitraryFunctionType *phi;
        ArbitraryFunctionType *dPhiDt;
    };
}

#endif //V_SHAPE_FIELDSTATE_H
