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
#include "Math/Data/Data.h"
#include "Utils/Map.h"

namespace Slab::Math::Base {


    template<class InCategory, class OutCategory>
    class NumericFunction : public FunctionT<InCategory, OutCategory>,
                            public NumericAlgebra<NumericFunction<InCategory, OutCategory>>,
                            public Data
    {
        TPointer<DiscreteSpace> space;

    protected:
        Device dev=CPU;

    public:
        using NumericAlgebra<Base::NumericFunction<InCategory, DevFloat>>::operator=;
        using NumericAlgebra<NumericFunction>::operator+=;

        typedef NumericFunction<InCategory, OutCategory> MyType;
        typedef FunctionT<InCategory, OutCategory> MyBase;
        typedef std::shared_ptr<NumericFunction<InCategory,OutCategory>> Ptr;

        Str generalName() const override { return "general discrete"; }

        DataType get_data_type() const override {
            DataType type = "f:?↦ℝ";

            if(space == nullptr) return type;

            fix d = space->getMetaData().getNDim();
            type = "f:ℝ" + Map<UInt, Str>{{1, ""}, {2,"²"}, {3, "³"}}[d] +"↦ℝ";

            return type;
        }

        NumericFunction(DimensionMetaData dim, Device dev) : MyBase(nullptr, true), Data(generalName()), dev(dev) {
            switch(dev){
                case Device::CPU:
                    space = New<DiscreteSpaceCPU>(dim);
                    break;
#if USE_CUDA
                case Device::GPU:
                    space = New<DiscreteSpaceGPU>(dim);
                    break;
#endif
                default:
                    throw Exception("Unknown device in instantiation of FunctionArbitrary.");
            }
        };

        NumericFunction(const NumericFunction& to_clone)
        : NumericFunction(to_clone.space.get()->getMetaData(), to_clone.dev) {

        }


        virtual ~NumericFunction() = default;

        virtual TPointer<NumericFunction> CloneWithSize(UInt N) const { NOT_IMPLEMENTED }
        virtual NumericFunction &Set(const MyBase &func) = 0;
        virtual NumericFunction &SetArb(const NumericFunction &func){
            space->setToValue(func.getSpace());

            return *this;
        }

        virtual NumericFunction &operator=(const MyBase &func) { this->Set(func); return *this; }
        virtual NumericFunction &operator=(const NumericFunction &func) { this->SetArb(func); return *this; }
        virtual NumericFunction &operator+=(const MyBase &func) { NOT_IMPLEMENTED_CLASS_METHOD };

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
                            const DevFloat a) override {
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
