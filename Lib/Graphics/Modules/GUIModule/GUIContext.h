//
// Created by joao on 11/3/24.
//

#ifndef STUDIOSLAB_GUICONTEXT_H
#define STUDIOSLAB_GUICONTEXT_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"

namespace Slab::Graphics {

    class GUIContext : public SystemWindowEventListener {
    public:
        virtual void NewFrame() = 0;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_GUICONTEXT_H
