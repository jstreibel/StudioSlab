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
    class NumericFunction : public FunctionT<InCategory, OutCategory>,
                            public NumericAlgebra<NumericFunction<InCategory, OutCategory>>
    {
        DiscreteSpace *space;

    protected:
        device dev;

    public:
        using NumericAlgebra<Base::NumericFunction<InCategory, Real>>::operator=;
        typedef NumericFunction<InCategory, OutCategory> MyType;
        typedef FunctionT<InCategory, OutCategory> MyBase;
        typedef std::shared_ptr<NumericFunction<InCategory,OutCategory>> Ptr;

        Str myName() const override { return "general discrete"; }

        NumericFunction(DimensionMetaData dim, device dev) : MyBase(nullptr, true), dev(dev) {
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
        virtual ~NumericFunction(){
            delete space;
        }

        virtual Pointer<NumericFunction> CloneWithSize(UInt N) const { NOT_IMPLEMENTED }
        virtual NumericFunction &Set(const MyBase &func) = 0;
        virtual NumericFunction &SetArb(const NumericFunction &func){
            space->setToValue(func.getSpace());

            return *this;
        }

        virtual NumericFunction &operator=(const MyBase &func) { this->Set(func); return *this; }
        virtual NumericFunction &operator=(const NumericFunction &func) { this->SetArb(func); return *this; }
        // virtual NumericFunction &operator*(const Real &val) { this->Multiply(val); return *this; }
        virtual NumericFunction &operator+=(const MyBase &func) { throw Str("ArbitraryFunction operator += not implemented"); };

        //ArbitraryFunction& operator-=(const ArbitraryFunction& rhs);
        //ArbitraryFunction& operator*=(const ArbitraryFunction& rhs);
        //friend ArbitraryFunction& operator+(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);
        //friend ArbitraryFunction& operator-(ArbitraryFunction& lhs, const ArbitraryFunction& rhs);


        // TODO: override o op. () para Function aqui abaixo (static), para chamar func(arb).
        virtual NumericFunction &Apply(const FunctionT<OutCategory, OutCategory> &func,
                                       NumericFunction &out) const = 0;

        auto Add(const NumericFunction<InCategory, OutCategory> &toi) ->
        NumericFunction<InCategory, OutCategory> & override {
            space->Add(*toi.space);
            return *this;
        }

        auto Subtract(const NumericFunction<InCategory, OutCategory> &toi) ->
        NumericFunction<InCategory, OutCategory> & override {
            space->Subtract(*toi.space);
            return *this;
        }

        auto StoreAddition(const NumericFunction<InCategory, OutCategory> &toi1,
                      const NumericFunction <InCategory, OutCategory> & toi2) ->
                      NumericFunction<InCategory, OutCategory> & override{
            space->StoreAddition(*toi1.space, *toi2.space);
            return *this;
        }

        auto StoreSubtraction(const NumericFunction<InCategory, OutCategory> &aoi1,
                         const NumericFunction<InCategory, OutCategory> & aoi2) ->
                         NumericFunction<InCategory, OutCategory> & override{
            space->StoreSubtraction(*aoi1.space, *aoi2.space);
            return *this;
        }

        NumericFunction<InCategory, OutCategory> &
        StoreScalarMultiplication(const NumericFunction<InCategory, OutCategory> &aoi1,
                            const Real a) override {
            space->StoreScalarMultiplication(*aoi1.space, a);
            return *this;
        }

        NumericFunction<InCategory, OutCategory> &Multiply(floatt a) override {
            space->Multiply(a);
            return *this;
        }

        auto getSpace()       ->       DiscreteSpace& { return *space; }
        auto getSpace() const -> const DiscreteSpace& { return *space; }

    };
}


#endif //V_SHAPE_FUNCTIONARBITRARY_H
