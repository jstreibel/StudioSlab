//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_WINDOWPROXY_H
#define STUDIOSLAB_WINDOWPROXY_H

#include "SlabWindow.h"

namespace Slab::Graphics {

    class WindowProxy : public SlabWindow {
        Pointer<SlabWindow> window;

    public:
        WindowProxy();

        void setWindow(Pointer<SlabWindow>);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_WINDOWPROXY_H
