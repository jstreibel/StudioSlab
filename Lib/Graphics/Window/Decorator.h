//
// Created by joao on 10/20/24.
//

#ifndef STUDIOSLAB_DECORATOR_H
#define STUDIOSLAB_DECORATOR_H

#include "SlabWindow.h"

namespace Slab::Graphics {

    class Decorator {
        int syswin_w, syswin_h;

    public:
        void operator()(const SlabWindow&);

        void setSystemWindowShape(int w, int h);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_DECORATOR_H
