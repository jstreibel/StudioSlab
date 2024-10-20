//
// Created by joao on 10/17/24.
//

#ifndef STUDIOSLAB_SYSTEMWINDOW_H
#define STUDIOSLAB_SYSTEMWINDOW_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Window/Window.h"

namespace Slab::Graphics {

    class SystemWindow {
        Vector<Pointer<SystemWindowEventListener>> eventListeners;
        Vector<Pointer<Window>> slab_windows;

    public:
        virtual void addEventListener(Pointer<SystemWindowEventListener>);

        virtual void addSlabWindow(Pointer<Window>);
    };

} // Slab::Core

#endif //STUDIOSLAB_SYSTEMWINDOW_H
