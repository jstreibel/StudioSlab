//
// Created by joao on 10/17/24.
//

#ifndef STUDIOSLAB_SYSTEMWINDOW_H
#define STUDIOSLAB_SYSTEMWINDOW_H

#include "Graphics/Backend/Events/GUIEventListener.h"

namespace Slab::Core {

    class SystemWindow {
        // Core::GUIEventListener

    public:
        virtual void addGUIEventListener();

        virtual void addWindow();
    };

} // Slab::Core

#endif //STUDIOSLAB_SYSTEMWINDOW_H
