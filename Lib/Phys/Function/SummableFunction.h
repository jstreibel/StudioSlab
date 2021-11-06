//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_SUMMABLEFUNCTION_H
#define STUDIOSLAB_SUMMABLEFUNCTION_H

#include "Function.h"


namespace Base {

    template<class InputCategory, class OutputCategory>
    class SummableFunction : public Function<InputCategory, OutputCategory> {
    private:
        typedef Function<InputCategory, OutputCategory> MyBase;

    public:
        SummableFunction() = default;

        SummableFunction(const SummableFunction &toClone) {
            for (const auto *func : toClone.funcs)
                funcs.push_back(func->Clone());
        }

        SummableFunction(const MyBase &func1, const MyBase &func2) {
            (*this += func1) += func2;
        }

        Function<InputCategory, OutputCategory> *Clone() const override { return new SummableFunction(*this); }

        SummableFunction &operator+=(const MyBase &func) {
            funcs.push_back(func.Clone());
            return *this;
        }

        typename MyBase::Pointer diff(int n) const override {
            auto *myDiff = new SummableFunction;

            for (auto *func : funcs)
                (*myDiff) += *(func->diff(n));

            return typename MyBase::Pointer(myDiff);
        }

        OutputCategory operator()(InputCategory x) const override {
            OutputCategory val(0);
            for (auto *func : funcs) {
                auto &f = *func;
                val += f(x);
            }

            return val;
        }

    private:
        std::vector<MyBase *> funcs;
    };

    template<class InCategory, class OutCategory>
    SummableFunction<InCategory, OutCategory>
    operator+(const Function<InCategory, OutCategory> &func1,
              const Function<InCategory, OutCategory> &func2) {
        return SummableFunction<InCategory, OutCategory>(func1, func2);
    }

}

#endif //STUDIOSLAB_SUMMABLEFUNCTION_H
