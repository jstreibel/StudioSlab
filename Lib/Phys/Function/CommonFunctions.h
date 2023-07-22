//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_COMMONFUNCTIONS_H
#define STUDIOSLAB_COMMONFUNCTIONS_H

#include "Function.h"
#include "SummableFunction.h"

namespace Base {

    template<class OutputCategory>
    struct Functions {
        typedef FunctionT<Real, OutputCategory> Function1D;
        typedef SummableFunction<Real, OutputCategory> Function1DSummable;

        typedef FunctionT<Real2D, OutputCategory> Function2D;
        typedef SummableFunction<Real, OutputCategory> Function2DSummable;

        typedef FunctionT<Real3D, OutputCategory> Function3D;
        // typedef Function<SU2<3>, OutputCategory> Function3D;
        // etc.
    };
}

#endif //STUDIOSLAB_COMMONFUNCTIONS_H
