//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_MODULE_H
#define STUDIOSLAB_MODULE_H

#include <memory> // Includes Shared Pointer
#include "Utils/String.h"

namespace Slab::Core {

    class Module {
    public:
        const Str name;
        const bool requiresGraphicsBackend;

        typedef std::shared_ptr<Module> Ptr;
        Module() = delete;
        explicit Module(Str name, bool requiresGraphicsBackend=false);
        virtual ~Module();
    };

    class GraphicsModule : public Module {
    public:
        GraphicsModule() = delete;
        explicit GraphicsModule(const Str &name) : Module(name, true) {}

        virtual void beginRender() {};
        virtual void endRender() {};

        virtual void beginEvents() {};
        virtual void endEvents() {};
    };

} // Core

#endif //STUDIOSLAB_MODULE_H
