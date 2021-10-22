//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_FUNCTIONSBASE_H
#define V_SHAPE_FUNCTIONSBASE_H

#include "Util/MathTypes.h"
#include "View/Util/FStateOutputInterface.h"

namespace Base {

    class GPUFriendly;

    template<class PosSpaceType, class TargetSpaceType>
    class Function {
    public:
        typedef std::unique_ptr<Function<PosSpaceType, TargetSpaceType> > FunctionPtr;
        typedef Function<PosSpaceType, TargetSpaceType> BaseType;
        typedef PosSpaceType PositionSpaceType;
        typedef TargetSpaceType TgtSpaceType;

    protected:
        /** If the function derived from this class has a GPUFriendly version, then it should provide it at instantiation
         * time, because a single copy of this function must be kept and be provided when needed. */
        explicit Function(const GPUFriendly *gpuFriendlyVersion = nullptr, bool isDiscrete = false)
                : myGPUFriendlyVersion(*gpuFriendlyVersion), discrete(isDiscrete) {}

        Function(const Function &toCopy) : myGPUFriendlyVersion(toCopy.getGPUFriendlyVersion()), discrete(toCopy.discrete){ }

    public:
        virtual ~Function() = default;

        virtual TargetSpaceType operator()(PosSpaceType x) const = 0;

        /*! Get derivative in dimension 'n'.
         * @param n: the number of the dimension to differentiate, e.g. n=0 => x, n=1 => y, etc.*/
        virtual FunctionPtr diff(int n) const { throw "Function::diff(n) not implemented."; };

        /*! Get derivative in dimension 'n' at point 'x'.
         * @param n: the number of the dimension to differentiate, e.g. n=0 => x, n=1 => y, etc.
         * @param x: the location to get differentiation. */
        virtual TargetSpaceType diff(int n, PosSpaceType x) const { throw "Function::diff(n, x) not implemented."; };

        /*!
         * Integrates the function through all of its domain.
         * @return The value of the integral.
         */
        virtual TargetSpaceType integrate() const { throw "Function::integrate not implemented."; };

        virtual Function *Clone() const { throw "Function::Clone() not implemented."; };

    public:
        virtual bool isDiscrete() const { return discrete; }

    private:
        const bool discrete;

    public:
        /** Returns a managed reference to a GPUFriendly version of this function. */
        const GPUFriendly &getGPUFriendlyVersion() const { return myGPUFriendlyVersion; };

        bool isGPUFriendly() const { return (&getGPUFriendlyVersion()) != nullptr; }

    protected:
        const GPUFriendly &myGPUFriendlyVersion;
    };


    template<class PosSpaceType, class TargetSpaceType>
    class FunctionBaseSummable : public Function<PosSpaceType, TargetSpaceType> {
    private:
        typedef Function<PosSpaceType, TargetSpaceType> MyBase;

    public:
        FunctionBaseSummable() = default;

        FunctionBaseSummable(const FunctionBaseSummable &toClone) {
            for (const auto *func : toClone.funcs)
                funcs.push_back(func->Clone());
        }

        FunctionBaseSummable(const MyBase &func1, const MyBase &func2) {
            (*this += func1) += func2;
        }

        Function<PosSpaceType, TargetSpaceType> *Clone() const override { return new FunctionBaseSummable(*this); }

        FunctionBaseSummable &operator+=(const MyBase &func) {
            funcs.push_back(func.Clone());
            return *this;
        }

        typename MyBase::FunctionPtr diff(int n) const override {
            auto *myDiff = new FunctionBaseSummable;

            for (auto *func : funcs)
                (*myDiff) += *(func->diff(n));

            return typename MyBase::FunctionPtr(myDiff);
        }

        TargetSpaceType operator()(PosSpaceType x) const override {
            TargetSpaceType val(0);
            for (auto *func : funcs) {
                auto &f = *func;
                val += f(x);
            }

            return val;
        }

    private:
        std::vector<MyBase *> funcs;
    };


    template<class PosSpaceType, class TargetSpaceType>
    FunctionBaseSummable<PosSpaceType, TargetSpaceType>
    operator+(const Function<PosSpaceType, TargetSpaceType> &func1,
              const Function<PosSpaceType, TargetSpaceType> &func2) {
        return FunctionBaseSummable<PosSpaceType, TargetSpaceType>(func1, func2);
    }

    template<class TargetSpaceType>
    struct Functions {
        typedef Function<Real, TargetSpaceType> Function1D;
        typedef FunctionBaseSummable<Real, TargetSpaceType> Function1DSummable;

        typedef Function<Real2D, TargetSpaceType> Function2D;
        typedef FunctionBaseSummable<Real, TargetSpaceType> Function2DSummable;

        typedef Function<Real3D, TargetSpaceType> Function3D;
        // typedef Function<SU2<3>, TargetSpaceType> Function3D;
        // etc.
    };

}


#endif //V_SHAPE_FUNCTIONSBASE_H
