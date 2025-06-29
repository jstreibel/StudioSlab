//
// Created by joao on 11/14/24.
//

#ifndef STUDIOSLAB_GUIMODULE_H
#define STUDIOSLAB_GUIMODULE_H

#include "Graphics/Modules/GraphicsModule.h"
#include "GUIContext.h"

namespace Slab::Graphics {

    class GUIModule : public GraphicsModule {
    public:
        explicit GUIModule(const Str& name, ParentSystemWindow);
        ~GUIModule() override = default;

        virtual auto CreateContext(ParentSystemWindow) -> Pointer<GUIContext> = 0;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_GUIMODULE_H
