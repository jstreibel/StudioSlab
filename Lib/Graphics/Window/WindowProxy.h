//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_WINDOWPROXY_H
#define STUDIOSLAB_WINDOWPROXY_H

#include "SlabWindow.h"

namespace Slab::Graphics {

    class WindowProxy : public FSlabWindow {
        TPointer<FSlabWindow> window;

    public:
        explicit WindowProxy(FSlabWindowConfig ConfigArg);

        void setWindow(TPointer<FSlabWindow>);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_WINDOWPROXY_H
