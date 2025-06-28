//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_FUNCTIONSCALE_H
#define STUDIOSLAB_FUNCTIONSCALE_H

#include "Function.h"

namespace Slab::Math::Base {

    template<class InputCategory, class OutputCategory>
    class Scale : public FunctionT<InputCategory, OutputCategory> {
        typedef FunctionT<InputCategory, OutputCategory> MyBase;

        const MyBase &A;
        DevFloat scale;

    public:
        Scale(const MyBase &a, DevFloat scale)
                : FunctionT<InputCategory, OutputCategory>(&a.getGPUFriendlyVersion(), a.isDiscrete()),
                  A(a), scale(scale) { }

        Scale(const Scale &toCopy) : Scale(toCopy.A, toCopy.scale) { }

        friend MyBase::Ptr operator*(const MyBase &a, DevFloat scale) {
            return std::make_unique(a, scale);
        }

        OutputCategory operator()(InputCategory x) const override { return A(x) * scale; }
    };
}

#endif //STUDIOSLAB_FUNCTIONSCALE_H
