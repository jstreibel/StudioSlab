//
// Created by joao on 11/14/24.
//

#ifndef STUDIOSLAB_GUIMODULE_H
#define STUDIOSLAB_GUIMODULE_H

#include "Graphics/Modules/GraphicsModule.h"
#include "GUIContext.h"

namespace Slab::Graphics {

    class FGUIModule : public GraphicsModule {
    public:
        explicit FGUIModule(const Str& name, FOwnerSystemWindow);
        ~FGUIModule() override = default;

        virtual auto CreateContext(FOwnerSystemWindow) -> Pointer<GUIContext> = 0;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_GUIMODULE_H
