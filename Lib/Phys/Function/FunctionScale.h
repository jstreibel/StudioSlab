//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_FUNCTIONSCALE_H
#define STUDIOSLAB_FUNCTIONSCALE_H

#include "Function.h"

namespace Base {

    template<class InputCategory, class OutputCategory>
    class Scale : public Function<InputCategory, OutputCategory> {
        typedef Function<InputCategory, OutputCategory> MyBase;

        const MyBase &A;
        Real scale;

    public:
        Scale(const MyBase &a, Real scale)
                : Function<InputCategory, OutputCategory>(&a.getGPUFriendlyVersion(), a.isDiscrete()),
                  A(a), scale(scale) { }

        Scale(const Scale &toCopy) : Scale(toCopy.A, toCopy.scale) { }

        friend MyBase::Ptr operator*(const MyBase &a, Real scale) {
            return std::make_unique(a, scale);
        }

        OutputCategory operator()(InputCategory x) const override { return A(x) * scale; }
    };
}

#endif //STUDIOSLAB_FUNCTIONSCALE_H
