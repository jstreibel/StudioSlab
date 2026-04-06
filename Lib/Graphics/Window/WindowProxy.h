//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_WINDOWPROXY_H
#define STUDIOSLAB_WINDOWPROXY_H

#include "SlabWindow.h"

namespace Slab::Graphics {

    class FWindowProxy : public FSlabWindow {
        TPointer<FSlabWindow> window;

    public:
        explicit FWindowProxy(FSlabWindowConfig ConfigArg);

        void setWindow(TPointer<FSlabWindow>);
    };

    using WindowProxy = FWindowProxy;

} // Slab::Graphics

#endif //STUDIOSLAB_WINDOWPROXY_H
