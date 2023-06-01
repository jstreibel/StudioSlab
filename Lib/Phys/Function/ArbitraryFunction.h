//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_FUNCTIONARBITRARY_H
#define V_SHAPE_FUNCTIONARBITRARY_H

#include "Function.h"

#include "Phys/Space/Impl/DiscreteSpace.h"
#include "Phys/Space/Impl/DiscreteSpaceCPU.h"
#include "Phys/Space/Impl/DiscreteSpaceGPU.h"

namespace Base {
    template<class PosSpaceType, class TargetSpaceType>

    class ArbitraryFunction : public Function<PosSpaceType, TargetSpaceType>,
                              public Utils::ArithmeticOpsInterface<ArbitraryFunction<PosSpaceType, TargetSpaceType>> {
    public:
        typedef Function<PosSpaceType, TargetSpaceType> MyBase;
    public:
        ArbitraryFunction(DimensionMetaData dim, Real h, device dev) : MyBase(nullptr, true) {
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
        virtual ~ArbitraryFunction(){
            delete space;
        }

        virtual ArbitraryFunction *CloneWithSize(PosInt N) const {throw "ModelBase::FunctionArbitrary::Clone()"
                                                                        " not implemented.";}
        virtual ArbitraryFunction &Set(const MyBase &func) = 0;
        virtual ArbitraryFunction &SetArb(const ArbitraryFunction &func){
            space->setToValue(func.getSpace());

            return *this;
        }

        virtual ArbitraryFunction &operator=(const MyBase &func) { (*this) = func; return *this; }
        virtual ArbitraryFunction &operator=(const ArbitraryFunction &func) { this->SetArb(func); return *this; }

        //ArbitraryFunction& operator+=(const ArbitraryFunction& rhs);
        //ArbitraryFunction& operator-=(const ArbitraryFunction& rhs);
        //ArbitraryFunction& operator*=(const ArbitraryFunction& rhs);
        //
        //friend ArbitraryFunction& operator+(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);
        //friend ArbitraryFunction& operator-(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);




        // TODO: override o op. () para Function aqui abaixo (static), para chamar func(arb).
        virtual ArbitraryFunction &Apply(const Function<TargetSpaceType, TargetSpaceType> &func,
                                         ArbitraryFunction &out) const = 0;

        auto Add(const ArbitraryFunction<PosSpaceType, TargetSpaceType> &toi) ->
        ArbitraryFunction<PosSpaceType, TargetSpaceType> & override {
            space->Add(*toi.space);
            return *this;
        }

        auto Subtract(const ArbitraryFunction<PosSpaceType, TargetSpaceType> &toi) ->
        ArbitraryFunction<PosSpaceType, TargetSpaceType> & override {
            space->Subtract(*toi.space);
            return *this;
        }

        auto StoreAddition(const ArbitraryFunction<PosSpaceType, TargetSpaceType> &toi1,
                      const ArbitraryFunction <PosSpaceType, TargetSpaceType> & toi2) ->
                      ArbitraryFunction<PosSpaceType, TargetSpaceType> & override{
            space->StoreAddition(*toi1.space, *toi2.space);
            return *this;
        }

        auto StoreSubtraction(const ArbitraryFunction<PosSpaceType, TargetSpaceType> &aoi1,
                         const ArbitraryFunction<PosSpaceType, TargetSpaceType> & aoi2) ->
                         ArbitraryFunction<PosSpaceType, TargetSpaceType> & override{
            space->StoreSubtraction(*aoi1.space, *aoi2.space);
            return *this;
        }

        ArbitraryFunction<PosSpaceType, TargetSpaceType> &
        StoreMultiplication(const ArbitraryFunction<PosSpaceType, TargetSpaceType> &aoi1,
                            const Real a) override {
            space->StoreMultiplication(*aoi1.space, a);
            return *this;
        }

        ArbitraryFunction<PosSpaceType, TargetSpaceType> &Multiply(floatt a) override {
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
