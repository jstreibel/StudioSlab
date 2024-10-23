//
// Created by joao on 10/17/24.
//

#ifndef STUDIOSLAB_SYSTEMWINDOW_H
#define STUDIOSLAB_SYSTEMWINDOW_H

#include "Utils/Numbers.h"

namespace Slab::Graphics {

    class SystemWindow {
    public:
        virtual Int getHeight() const = 0;
    };

} // Slab::Core

#endif //STUDIOSLAB_SYSTEMWINDOW_H
