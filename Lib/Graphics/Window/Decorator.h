//
// Created by joao on 10/20/24.
//

#ifndef STUDIOSLAB_DECORATOR_H
#define STUDIOSLAB_DECORATOR_H

#include "SlabWindow.h"

namespace Slab::Graphics {

    class Decorator {
    public:
        void operator()(const SlabWindow&);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_DECORATOR_H
