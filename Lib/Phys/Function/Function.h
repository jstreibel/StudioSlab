//
// Created by joao on 29/08/2019.
//

#ifndef V_SHAPE_FUNCTIONSBASE_H
#define V_SHAPE_FUNCTIONSBASE_H

#include "Phys/Formalism/Categories.h"
#include "Phys/Space/Space.h"
#include "Phys/Space/Impl/PointSet.h"

#include <Common/Types.h>

//#include "View/Util/FStateOutputInterface.h"

namespace Base {


    class GPUFriendly;


    template<class InputCategory, class OutputCategory>
    class Function {
    public:
        typedef std::shared_ptr<Function<InputCategory, OutputCategory>> Ptr;

        typedef Function<InputCategory, OutputCategory> Type;
        typedef InputCategory InCategory;
        typedef OutputCategory OutCategory;

        const bool discrete;

        Space *positionSpace;

    protected:
        const GPUFriendly &myGPUFriendlyVersion;

        /** If the function derived from this class has a GPUFriendly version, then it should provide it at instantiation
         * time, because a single copy of this function must be kept and be provided when needed. */
        explicit Function(const GPUFriendly *gpuFriendlyVersion = nullptr,
                          bool isDiscrete = false)
                          : myGPUFriendlyVersion(*gpuFriendlyVersion), discrete(isDiscrete)
                          {     }

        Function(const Function &toCopy)
                          : myGPUFriendlyVersion(toCopy.getGPUFriendlyVersion()),
                          discrete(toCopy.discrete)
                          {     }

    public:
        virtual ~Function() = default;

        virtual OutputCategory operator()(InputCategory x) const = 0;

        /*! Get derivative in dimension 'n'.
         * @param n: the number of the dimension to differentiate, e.g. n=0 => x, n=1 => y, etc.*/
        virtual Ptr diff(int n) const { throw "Function::diff(n) not implemented."; };

        /*! Get derivative in dimension 'n' at point 'x'.
         * @param n: the number of the dimension to differentiate, e.g. n=0 => x, n=1 => y, etc.
         * @param x: the location to get differentiation. */
        virtual OutputCategory diff(int n, InputCategory x) const { throw "Function::diff(n, x) not implemented."; }

        virtual bool domainContainsPoint(InputCategory x) const {return true;}

        /*!
         * Integrates the function through all of its domain.
         * @return The value of the integral.
         */
        virtual OutputCategory integrate() const { throw "Function::integrate not implemented."; }

        virtual Function *Clone() const { throw "Function::Clone() not implemented."; }

        virtual bool isDiscrete() const { return discrete; }

        /** Returns a managed reference to a GPUFriendly version of this function. */
        const GPUFriendly &getGPUFriendlyVersion() const { return myGPUFriendlyVersion; }

        bool isGPUFriendly() const { return (&getGPUFriendlyVersion()) != nullptr; }

        virtual String myName() const { return "unnamed"; }

        struct PointSetRenderingOptions {
            PointSetRenderingOptions()                        : hint(UseChoiceResolution)            {}
            PointSetRenderingOptions(const InputCategory &dx) : hint(UseCustomResolution_dx), dx(dx) {};
            PointSetRenderingOptions(PosInt n)                : hint(UseCustomResolution_n),  n(n)   {};


            const enum Hint {
                UseChoiceResolution,
                UseCustomResolution_dx,
                UseCustomResolution_n,
            } hint = UseChoiceResolution;

            InputCategory dx;
            PosInt n;
        };

        virtual Spaces::PointSet::Ptr
        renderPointSet(PointSetRenderingOptions options = PointSetRenderingOptions()) {
            throw "Not implemented.";
        };

    };
}


#endif //V_SHAPE_FUNCTIONSBASE_H
