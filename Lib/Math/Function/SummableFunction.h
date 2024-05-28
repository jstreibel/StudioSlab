//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SUMMABLEFUNCTION_H
#define STUDIOSLAB_SUMMABLEFUNCTION_H

#include "Function.h"


namespace Slab::Math::Base {

    template<class InputCategory, class OutputCategory>
    class SummableFunction : public FunctionT<InputCategory, OutputCategory> {
    private:
        typedef FunctionT<InputCategory, OutputCategory> MyBase;

    public:
        SummableFunction() = default;

        SummableFunction(const SummableFunction &toClone) {
            for (const auto &func : toClone.funcs)
                funcs.push_back(func->Clone());
        }

        SummableFunction(const MyBase &func1, const MyBase &func2) {
            (*this += func1) += func2;
        }

        Pointer<MyBase> Clone() const override { return New <SummableFunction> (*this); }

        SummableFunction &operator+=(const MyBase &func) {
            funcs.push_back(func.Clone());
            return *this;
        }

        Pointer<MyBase> diff(int n) const override {
            auto *myDiff = new SummableFunction;

            for (auto &func : funcs)
                (*myDiff) += *(func->diff(n));

            return Pointer<MyBase>(myDiff);
        }

        OutputCategory operator()(InputCategory x) const override {
            OutputCategory val(0);
            for (auto &func : funcs) {
                auto &f = *func;
                val += f(x);
            }

            return val;
        }

    private:
        Vector<Pointer<MyBase>> funcs;
    };

    template<class InCategory, class OutCategory>
    SummableFunction<InCategory, OutCategory>
    operator+(const FunctionT<InCategory, OutCategory> &func1,
              const FunctionT<InCategory, OutCategory> &func2) {
        return SummableFunction<InCategory, OutCategory>(func1, func2);
    }

}

#endif //STUDIOSLAB_SUMMABLEFUNCTION_H
