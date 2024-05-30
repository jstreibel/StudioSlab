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


    template<class InCategory, class OutCategory>
    class DiscreteFunction : public FunctionT<InCategory, OutCategory>,
                             public NumericAlgebra<DiscreteFunction<InCategory, OutCategory>>
    {
        DiscreteSpace *space;

    protected:
        device dev;

    public:
        using NumericAlgebra<Base::DiscreteFunction<InCategory, Real>>::operator=;
        typedef DiscreteFunction<InCategory, OutCategory> MyType;
        typedef FunctionT<InCategory, OutCategory> MyBase;
        typedef std::shared_ptr<DiscreteFunction<InCategory,OutCategory>> Ptr;

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
        virtual DiscreteFunction &operator=(const DiscreteFunction &func) { this->SetArb(func); return *this; }
        // virtual DiscreteFunction &operator*(const Real &val) { this->Multiply(val); return *this; }
        virtual DiscreteFunction &operator+=(const MyBase &func) { throw Str("ArbitraryFunction operator += not implemented"); };

        //ArbitraryFunction& operator-=(const ArbitraryFunction& rhs);
        //ArbitraryFunction& operator*=(const ArbitraryFunction& rhs);
        //friend ArbitraryFunction& operator+(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);
        //friend ArbitraryFunction& operator-(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);


        // TODO: override o op. () para Function aqui abaixo (static), para chamar func(arb).
        virtual DiscreteFunction &Apply(const FunctionT<OutCategory, OutCategory> &func,
                                        DiscreteFunction &out) const = 0;

        auto Add(const DiscreteFunction<InCategory, OutCategory> &toi) ->
        DiscreteFunction<InCategory, OutCategory> & override {
            space->Add(*toi.space);
            return *this;
        }

        auto Subtract(const DiscreteFunction<InCategory, OutCategory> &toi) ->
        DiscreteFunction<InCategory, OutCategory> & override {
            space->Subtract(*toi.space);
            return *this;
        }

        auto StoreAddition(const DiscreteFunction<InCategory, OutCategory> &toi1,
                      const DiscreteFunction <InCategory, OutCategory> & toi2) ->
                      DiscreteFunction<InCategory, OutCategory> & override{
            space->StoreAddition(*toi1.space, *toi2.space);
            return *this;
        }

        auto StoreSubtraction(const DiscreteFunction<InCategory, OutCategory> &aoi1,
                         const DiscreteFunction<InCategory, OutCategory> & aoi2) ->
                         DiscreteFunction<InCategory, OutCategory> & override{
            space->StoreSubtraction(*aoi1.space, *aoi2.space);
            return *this;
        }

        DiscreteFunction<InCategory, OutCategory> &
        StoreScalarMultiplication(const DiscreteFunction<InCategory, OutCategory> &aoi1,
                            const Real a) override {
            space->StoreScalarMultiplication(*aoi1.space, a);
            return *this;
        }

        DiscreteFunction<InCategory, OutCategory> &Multiply(floatt a) override {
            space->Multiply(a);
            return *this;
        }

        auto getSpace()       ->       DiscreteSpace& { return *space; }
        auto getSpace() const -> const DiscreteSpace& { return *space; }

    };
}


#endif //V_SHAPE_FUNCTIONARBITRARY_H
