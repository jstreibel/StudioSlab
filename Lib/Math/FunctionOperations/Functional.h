//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_FUNCTIONAL_H
#define STUDIOSLAB_FUNCTIONAL_H

#include "Utils/Types.h"

namespace Slab::Math {

    template < class FunctionType >
    class Functional {
    public:
        virtual Real operator[] (FunctionType &function) const = 0;

    };
}


#endif //STUDIOSLAB_FUNCTIONAL_H
