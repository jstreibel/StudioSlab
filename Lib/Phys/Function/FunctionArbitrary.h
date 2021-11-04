//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_FUNCTIONARBITRARY_H
#define V_SHAPE_FUNCTIONARBITRARY_H

#include "FunctionsBase.h"

#include "Phys/Space/DiscreteSpace.h"
#include "Phys/Space/DiscreteSpaceCPU.h"
#include "Phys/Space/DiscreteSpaceGPU.h"

namespace Base {
    template<class PosSpaceType, class TargetSpaceType>

    class FunctionArbitrary : public Function<PosSpaceType, TargetSpaceType>,
                              public Utils::ArithmeticOpsInterface<FunctionArbitrary<PosSpaceType, TargetSpaceType>> {
    public:
        typedef Function<PosSpaceType, TargetSpaceType> MyBase;
    public:
        FunctionArbitrary(DimensionMetaData dim, Real h, device dev) : MyBase(nullptr, true) {
            switch(dev){
                case device::CPU:
                    space = new DiscreteSpaceCPU(dim, h);
                    break;
#if USE_CUDA
                case device::GPU:
                    space = new DiscreteSpaceGPU(dim, h);
                    break;
#endif
                default:
                    throw "Unknown device in instantiation of FunctionArbitrary.";
            }
        };

        virtual FunctionArbitrary *CloneWithSize(PosInt N) const {throw "ModelBase::FunctionArbitrary::Clone() not implemented.";}
        virtual FunctionArbitrary &Set(const MyBase &func) = 0;
        virtual FunctionArbitrary &SetArb(const FunctionArbitrary &func){
            space->setToValue(func.getSpace());

            return *this;
        }

        FunctionArbitrary &operator=(const MyBase &func) { (*this) = func; }

        // TODO: override o op. () para Function aqui abaixo (static), para chamar func(arb).
        virtual FunctionArbitrary &Apply(const Function<TargetSpaceType,TargetSpaceType> &func, FunctionArbitrary &out) const = 0;

        auto Add(const FunctionArbitrary<PosSpaceType, TargetSpaceType> &toi) -> FunctionArbitrary<PosSpaceType, TargetSpaceType> & override {
            space->Add(*toi.space);
            return *this;
        }
        auto StoreAddition(const FunctionArbitrary<PosSpaceType, TargetSpaceType> &toi1,
                      const FunctionArbitrary <PosSpaceType, TargetSpaceType> & toi2) -> FunctionArbitrary<PosSpaceType, TargetSpaceType> & override{
            space->StoreAddition(*toi1.space, *toi2.space);
            return *this;
        }
        auto StoreSubtraction(const FunctionArbitrary<PosSpaceType, TargetSpaceType> &aoi1,
                         const FunctionArbitrary<PosSpaceType, TargetSpaceType> & aoi2) -> FunctionArbitrary<PosSpaceType, TargetSpaceType> & override{
            space->StoreSubtraction(*aoi1.space, *aoi2.space);
            return *this;
        }
        FunctionArbitrary<PosSpaceType, TargetSpaceType> &Multiply(floatt a) override {
            space->Multiply(a);
            return *this;
        }

        const DiscreteSpace& getSpace() const {
            return *space;
        }

        DiscreteSpace& getSpace() {
            return *space;
        }

    private:
        DiscreteSpace *space;

    };

}


#endif //V_SHAPE_FUNCTIONARBITRARY_H
