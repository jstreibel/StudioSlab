//
// Created by joao on 10/17/24.
//

#ifndef STUDIOSLAB_GRAPHICSMODULE_H
#define STUDIOSLAB_GRAPHICSMODULE_H

#include "Core/Backend/Modules/Module.h"

#include "Graphics/Backend/Events/SystemWindowEventListener.h"

namespace Slab::Graphics {

    class FGraphicsModule : public Core::SlabModule, public FPlatformWindowEventListener {
    public:
        FGraphicsModule() = delete;
        explicit FGraphicsModule(const Str &name) : SlabModule(name, true) {}

        virtual
        void Update() {};
    };

    using GraphicsModule [[deprecated("Use FGraphicsModule")]] = FGraphicsModule;

} // Slab::Graphics

#endif //STUDIOSLAB_GRAPHICSMODULE_H
