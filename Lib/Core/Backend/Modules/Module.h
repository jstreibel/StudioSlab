//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_MODULE_H
#define STUDIOSLAB_MODULE_H

#include "Core/Backend/Events/GUIEventListener.h"

namespace Core {

    class Module : public GUIEventListener {
    public:
        virtual void beginRender() = 0;
        virtual void endRender() = 0;
    };

} // Core

#endif //STUDIOSLAB_MODULE_H
