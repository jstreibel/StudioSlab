//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_FUNCTIONSBASE_H
#define V_SHAPE_FUNCTIONSBASE_H

#include "Phys/Formalism/Categories.h"
#include "Phys/Space/Space.h"
#include "Phys/Space/Impl/PointSet.h"

#include <Common/Types.h>

//#include "View/Util/EqStateOutputInterface.h"

namespace Base {


    class GPUFriendly;

    template<class InputCategory, class OutputCategory>
    class DiscreteFunction;

    class Function {
    public:
        // virtual Category& operator()(Category&){ throw "Function::operator() not implemented."; };
    };

    template<class InputCategory, class OutputCategory>
    class FunctionT : public Function {
    public:
        typedef std::shared_ptr<FunctionT<InputCategory, OutputCategory>> Ptr;
        typedef std::shared_ptr<const FunctionT<InputCategory, OutputCategory>> ConstPtr;

        typedef FunctionT<InputCategory, OutputCategory> Type;
        typedef InputCategory InCategory;
        typedef OutputCategory OutCategory;

        const bool discrete;

    protected:
        const GPUFriendly &myGPUFriendlyVersion;

        /** If the function derived from this class has a GPUFriendly version, then it should provide it at instantiation
         * time, because a single copy of this function must be kept and be provided when needed. */
        explicit FunctionT(const GPUFriendly *gpuFriendlyVersion = nullptr,
                           bool isDiscrete = false)
                          : myGPUFriendlyVersion(*gpuFriendlyVersion), discrete(isDiscrete)
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
        const GPUFriendly &getGPUFriendlyVersion() const { return myGPUFriendlyVersion; }

        bool isGPUFriendly() const { return (&getGPUFriendlyVersion()) != nullptr; }

        virtual Str myName() const { return "unnamed"; }

        // RENDERING
        struct RenderingOptions {
            RenderingOptions()                        : hint(UseChoiceResolution)            {}
            RenderingOptions(const InputCategory &dx) : hint(UseCustomResolution_dx), dx(dx) {};
            RenderingOptions(PosInt n)                : hint(UseCustomResolution_n), n(n)   {};


            const enum Hint {
                UseChoiceResolution,
                UseCustomResolution_dx,
                UseCustomResolution_n,
            } hint = UseChoiceResolution;

            InputCategory dx;
            PosInt n;
        };

        virtual Spaces::PointSet::Ptr
        renderToPointSet(RenderingOptions options = RenderingOptions()) {
            throw "Not implemented.";
        };

        virtual bool
        renderToDiscreteFunction(DiscreteFunction<InputCategory, OutputCategory> *toFunc) const {
            throw Str("Function '") + myName() + "' method renderToDiscreteFunction not implemented.";
        };

    };
}


#endif //V_SHAPE_FUNCTIONSBASE_H
