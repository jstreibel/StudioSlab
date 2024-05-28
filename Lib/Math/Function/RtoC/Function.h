//
// Created by joao on 11/09/23.
//

#ifndef STUDIOSLAB_RTOCFUNCTION_H
#define STUDIOSLAB_RTOCFUNCTION_H


#include "Utils/Types.h"
#include "Math/Function/CommonFunctions.h"

namespace Slab::Math::RtoC {
    typedef Base::Functions<Complex>::Function1D Function;

    DefinePointer(Function)
}

#endif //STUDIOSLAB_RTOCFUNCTION_H
