//
// Created by joao on 10/17/24.
//

#ifndef STUDIOSLAB_GRAPHICSMODULE_H
#define STUDIOSLAB_GRAPHICSMODULE_H

#include "Core/Backend/Modules/Module.h"

#include "Graphics/Backend/Events/SystemWindowEventListener.h"

namespace Slab::Graphics {

    class GraphicsModule : public Core::Module, public SystemWindowEventListener {
    public:
        GraphicsModule() = delete;
        explicit GraphicsModule(const Str &name, ParentSystemWindow parent=nullptr)
        : Module(name, true), SystemWindowEventListener(parent) {}

        virtual
        void Update() {};
    };

} // Slab::Graphics

#endif //STUDIOSLAB_GRAPHICSMODULE_H
