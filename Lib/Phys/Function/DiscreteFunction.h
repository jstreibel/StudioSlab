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
    class DiscreteFunction : public FunctionT<PosSpaceType, TargetSpaceType>,
                             public Utils::ArithmeticOpsInterface<DiscreteFunction<PosSpaceType, TargetSpaceType>> {

    public:
        typedef FunctionT<PosSpaceType, TargetSpaceType> MyBase;
        typedef std::shared_ptr<DiscreteFunction<PosSpaceType,TargetSpaceType>> Ptr;

        DiscreteFunction(DimensionMetaData dim, Real h, device dev) : MyBase(nullptr, true) {
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
        virtual ~DiscreteFunction(){
            delete space;
        }

        virtual DiscreteFunction *CloneWithSize(PosInt N) const {throw "ModelBase::FunctionArbitrary::Clone()"
                                                                        " not implemented.";}
        virtual DiscreteFunction &Set(const MyBase &func) = 0;
        virtual DiscreteFunction &SetArb(const DiscreteFunction &func){
            space->setToValue(func.getSpace());

            return *this;
        }

//         virtual DiscreteFunction &operator=(const MyBase &func) { (*this) = func; return *this; }
        virtual DiscreteFunction &operator=(const DiscreteFunction &func) { this->SetArb(func); return *this; }

        virtual DiscreteFunction &operator+=(const MyBase &func) { throw "ArbitraryFunction operator += not implemented"; };

        //ArbitraryFunction& operator-=(const ArbitraryFunction& rhs);
        //ArbitraryFunction& operator*=(const ArbitraryFunction& rhs);
        //
        //friend ArbitraryFunction& operator+(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);
        //friend ArbitraryFunction& operator-(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);




        // TODO: override o op. () para Function aqui abaixo (static), para chamar func(arb).
        virtual DiscreteFunction &Apply(const FunctionT<TargetSpaceType, TargetSpaceType> &func,
                                        DiscreteFunction &out) const = 0;

        auto Add(const DiscreteFunction<PosSpaceType, TargetSpaceType> &toi) ->
        DiscreteFunction<PosSpaceType, TargetSpaceType> & override {
            space->Add(*toi.space);
            return *this;
        }

        auto Subtract(const DiscreteFunction<PosSpaceType, TargetSpaceType> &toi) ->
        DiscreteFunction<PosSpaceType, TargetSpaceType> & override {
            space->Subtract(*toi.space);
            return *this;
        }

        auto StoreAddition(const DiscreteFunction<PosSpaceType, TargetSpaceType> &toi1,
                      const DiscreteFunction <PosSpaceType, TargetSpaceType> & toi2) ->
                      DiscreteFunction<PosSpaceType, TargetSpaceType> & override{
            space->StoreAddition(*toi1.space, *toi2.space);
            return *this;
        }

        auto StoreSubtraction(const DiscreteFunction<PosSpaceType, TargetSpaceType> &aoi1,
                         const DiscreteFunction<PosSpaceType, TargetSpaceType> & aoi2) ->
                         DiscreteFunction<PosSpaceType, TargetSpaceType> & override{
            space->StoreSubtraction(*aoi1.space, *aoi2.space);
            return *this;
        }

        DiscreteFunction<PosSpaceType, TargetSpaceType> &
        StoreMultiplication(const DiscreteFunction<PosSpaceType, TargetSpaceType> &aoi1,
                            const Real a) override {
            space->StoreMultiplication(*aoi1.space, a);
            return *this;
        }

        DiscreteFunction<PosSpaceType, TargetSpaceType> &Multiply(floatt a) override {
            space->Multiply(a);
            return *this;
        }

        auto getSpace()       ->       DiscreteSpace& { return *space; }
        auto getSpace() const -> const DiscreteSpace& { return *space; }


    private:
        DiscreteSpace *space;

    };

}


#endif //V_SHAPE_FUNCTIONARBITRARY_H
