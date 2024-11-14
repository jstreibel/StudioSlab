//
// Created by joao on 11/14/24.
//

#ifndef STUDIOSLAB_GUIMODULE_H
#define STUDIOSLAB_GUIMODULE_H

#include "Graphics/Modules/GraphicsModule.h"
#include "GUIContext.h"

namespace Slab::Graphics {

    using RawSystemWindowPointer = void*;

    class GUIModule : public GraphicsModule {
    public:
        explicit GUIModule(const Str& name);
        ~GUIModule() override = default;

        virtual auto createContext(RawSystemWindowPointer) -> Pointer<GUIContext> = 0;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_GUIMODULE_H
