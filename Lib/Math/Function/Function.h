//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_FUNCTIONSBASE_H
#define V_SHAPE_FUNCTIONSBASE_H

#include "Math/Formalism/Categories.h"
#include "Math/Space/Space.h"
#include "Math/Space/Impl/PointSet.h"

#include <Utils/Types.h>

//#include "View/Util/EqStateOutputInterface.h"

namespace Core {


    class GPUFriendly;

    template<class InputCategory, class OutputCategory>
    class DiscreteFunction;

    class Function {
    public:
        // virtual Category& operator()(Category&){ throw "Function::operator() not implemented."; };
    };

    template<class InputCategory, class OutputCategory>
    class FunctionT : public Function {
        bool discrete;
        const GPUFriendly *myGPUFriendlyVersion;

    public:
        typedef std::shared_ptr<FunctionT<InputCategory, OutputCategory>> Ptr;
        typedef std::shared_ptr<const FunctionT<InputCategory, OutputCategory>> ConstPtr;

        typedef FunctionT<InputCategory, OutputCategory> Type;
        typedef InputCategory InCategory;
        typedef OutputCategory OutCategory;


    protected:

        /** If the function derived from this class has a GPUFriendly version, then it should provide it at instantiation
         * time, because a single copy of this function must be kept and be provided when needed. */
        explicit FunctionT(const GPUFriendly *gpuFriendlyVersion = nullptr,
                           bool isDiscrete = false)
                          : myGPUFriendlyVersion(gpuFriendlyVersion), discrete(isDiscrete)
                          {     }

        FunctionT(const FunctionT &toCopy)
                          : myGPUFriendlyVersion(toCopy.getGPUFriendlyVersion()),
                          discrete(toCopy.discrete)
                          {     }

    public:
        virtual ~FunctionT() = default;

        virtual OutputCategory operator()(InputCategory x) const = 0;

        /*! Get derivative in dimension 'n'.
         * @param n: the number of the dimension to differentiate, e.g. n=0 => x, n=1 => y, etc.*/
        virtual Ptr diff(int n) const { throw Common::getClassName(this) + "::" + __FUNCTION__ + " is not implemented."; };

        /*! Get derivative in dimension 'n' at point 'x'.
         * @param n: the number of the dimension to differentiate, e.g. n=0 => x, n=1 => y, etc.
         * @param x: the location to get differentiation. */
        virtual OutputCategory diff(int n, InputCategory x) const { throw "Function::diff(n, x) not implemented."; }

        virtual auto domainContainsPoint(InputCategory x) const -> bool {return true;}

        /*!
         * Integrates the function through all of its domain.
         * @return The value of the integral.
         */
        virtual auto integrate()    const -> OutputCategory { throw "Function::integrate not implemented."; }
        virtual auto Clone()        const -> FunctionT *     { throw Str("Function::Clone() of function '") + myName() + "' not implemented."; }
        virtual auto isDiscrete()   const -> bool           { return discrete; }

        /** Returns a managed reference to a GPUFriendly version of this function. */
        const GPUFriendly &getGPUFriendlyVersion() const { return *myGPUFriendlyVersion; }

        bool isGPUFriendly() const { return (&getGPUFriendlyVersion()) != nullptr; }

        virtual Str myName() const { return "unnamed"; }

        // RENDERING
        struct RenderingOptions {
            RenderingOptions()
            : hint(UseChoiceResolution)
            { }

            RenderingOptions(const InputCategory &dx,
                             const InputCategory &xMin,
                             const InputCategory &xMax)
            : hint(UseCustomResolution_dx)
            , dx(dx)
            , n((xMax-xMin)/dx)
            , xMin(xMin)
            , xMax(xMax)
            { }

            RenderingOptions(PosInt n,
                             const InputCategory &xMin,
                             const InputCategory &xMax)
            : hint(UseCustomResolution_n)
            , dx((xMax-xMin)/n)
            , n(n)
            , xMin(xMin)
            , xMax(xMax)
            { }


            const enum Hint {
                UseChoiceResolution,
                UseCustomResolution_dx,
                UseCustomResolution_n,
            } hint = UseChoiceResolution;

            InputCategory dx;
            PosInt n{};
            InputCategory xMin, xMax;
        };

        virtual Spaces::PointSet::Ptr
        renderToPointSet(RenderingOptions options = RenderingOptions()) {
            throw Str("Function '") + myName() + "' method " + __PRETTY_FUNCTION__ + " not implemented.";
        };

        virtual bool
        renderToDiscreteFunction(DiscreteFunction<InputCategory, OutputCategory> *toFunc) const {
            throw Str("Function '") + myName() + "' method " + __PRETTY_FUNCTION__ + " not implemented.";
        };

    };
}


#endif //V_SHAPE_FUNCTIONSBASE_H