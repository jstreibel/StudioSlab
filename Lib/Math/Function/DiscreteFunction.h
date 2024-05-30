//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_FUNCTIONARBITRARY_H
#define V_SHAPE_FUNCTIONARBITRARY_H

#include "Function.h"

#include "Math/VectorSpace/Impl/DiscreteSpace.h"
#include "Math/VectorSpace/Impl/DiscreteSpaceCPU.h"
#include "Math/VectorSpace/Impl/DiscreteSpaceGPU.h"
#include "Core/Tools/Log.h"

namespace Slab::Math::Base {


    template<class ArgumentCategory, class FunctionCategory>
    class DiscreteFunction : public FunctionT<ArgumentCategory, FunctionCategory>,
                             public NumericAlgebra<DiscreteFunction<ArgumentCategory, FunctionCategory>>
    {
        DiscreteSpace *space;

    protected:
        device dev;

    public:
        using NumericAlgebra<Base::DiscreteFunction<ArgumentCategory, Real>>::operator=;
        typedef DiscreteFunction<ArgumentCategory, FunctionCategory> MyType;
        typedef FunctionT<ArgumentCategory, FunctionCategory> MyBase;
        typedef std::shared_ptr<DiscreteFunction<ArgumentCategory,FunctionCategory>> Ptr;

        Str myName() const override { return "general discrete"; }

        DiscreteFunction(DimensionMetaData dim, device dev) : MyBase(nullptr, true), dev(dev) {
            switch(dev){
                case device::CPU:
                    space = new DiscreteSpaceCPU(dim);
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

        virtual Pointer<DiscreteFunction> CloneWithSize(UInt N) const { NOT_IMPLEMENTED }
        virtual DiscreteFunction &Set(const MyBase &func) = 0;
        virtual DiscreteFunction &SetArb(const DiscreteFunction &func){
            space->setToValue(func.getSpace());

            return *this;
        }

        virtual DiscreteFunction &operator=(const MyBase &func) { this->Set(func); return *this; }
        // virtual DiscreteFunction &operator=(const DiscreteFunction &func) { this->SetArb(func); return *this; }
        // virtual DiscreteFunction &operator*(const Real &val) { this->Multiply(val); return *this; }
        virtual DiscreteFunction &operator+=(const MyBase &func) { throw Str("ArbitraryFunction operator += not implemented"); };

        //ArbitraryFunction& operator-=(const ArbitraryFunction& rhs);
        //ArbitraryFunction& operator*=(const ArbitraryFunction& rhs);
        //friend ArbitraryFunction& operator+(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);
        //friend ArbitraryFunction& operator-(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);


        // TODO: override o op. () para Function aqui abaixo (static), para chamar func(arb).
        virtual DiscreteFunction &Apply(const FunctionT<FunctionCategory, FunctionCategory> &func,
                                        DiscreteFunction &out) const = 0;

        auto Add(const DiscreteFunction<ArgumentCategory, FunctionCategory> &toi) ->
        DiscreteFunction<ArgumentCategory, FunctionCategory> & override {
            space->Add(*toi.space);
            return *this;
        }

        auto Subtract(const DiscreteFunction<ArgumentCategory, FunctionCategory> &toi) ->
        DiscreteFunction<ArgumentCategory, FunctionCategory> & override {
            space->Subtract(*toi.space);
            return *this;
        }

        auto StoreAddition(const DiscreteFunction<ArgumentCategory, FunctionCategory> &toi1,
                      const DiscreteFunction <ArgumentCategory, FunctionCategory> & toi2) ->
                      DiscreteFunction<ArgumentCategory, FunctionCategory> & override{
            space->StoreAddition(*toi1.space, *toi2.space);
            return *this;
        }

        auto StoreSubtraction(const DiscreteFunction<ArgumentCategory, FunctionCategory> &aoi1,
                         const DiscreteFunction<ArgumentCategory, FunctionCategory> & aoi2) ->
                         DiscreteFunction<ArgumentCategory, FunctionCategory> & override{
            space->StoreSubtraction(*aoi1.space, *aoi2.space);
            return *this;
        }

        DiscreteFunction<ArgumentCategory, FunctionCategory> &
        StoreScalarMultiplication(const DiscreteFunction<ArgumentCategory, FunctionCategory> &aoi1,
                            const Real a) override {
            space->StoreScalarMultiplication(*aoi1.space, a);
            return *this;
        }

        DiscreteFunction<ArgumentCategory, FunctionCategory> &Multiply(floatt a) override {
            space->Multiply(a);
            return *this;
        }

        auto getSpace()       ->       DiscreteSpace& { return *space; }
        auto getSpace() const -> const DiscreteSpace& { return *space; }

    };
}


#endif //V_SHAPE_FUNCTIONARBITRARY_H
