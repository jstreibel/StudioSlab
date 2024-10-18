//
// Created by joao on 10/17/24.
//

#ifndef STUDIOSLAB_GRAPHICSMODULE_H
#define STUDIOSLAB_GRAPHICSMODULE_H

#include "Core/Backend/Modules/Module.h"

namespace Slab::Graphics {

    class GraphicsModule : public Core::Module {
    public:
        GraphicsModule() = delete;
        explicit GraphicsModule(const Str &name) : Module(name, true) {}

        virtual void beginRender() {};
        virtual void endRender() {};

        virtual void beginEvents() {};
        virtual void endEvents() {};
    };

} // Slab::Graphics

#endif //STUDIOSLAB_GRAPHICSMODULE_H
