//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_MODULE_H
#define STUDIOSLAB_MODULE_H

#include <memory> // Includes Shared Pointer

namespace Slab::Core {

    class Module {
    public:
        typedef std::shared_ptr<Module> Ptr;

        virtual ~Module() = default;

        virtual void beginRender() {};
        virtual void endRender() {};

        virtual void beginEvents() {};
        virtual void endEvents() {};
    };

} // Core

#endif //STUDIOSLAB_MODULE_H
