//
// Created by joao on 29/05/23.
//

#ifndef STUDIOSLAB_FUNCTIONAL_H
#define STUDIOSLAB_FUNCTIONAL_H

#include "Common/CoreNativeTypes.h"

namespace Phys {
    namespace Functionals {

        template < class FunctionType >
        class Functional {

        public:

            virtual Real operator[] (FunctionType &function) const = 0;

        };

    }

}


#endif //STUDIOSLAB_FUNCTIONAL_H
