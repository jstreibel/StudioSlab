//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_GRAPHICSMODULE_H
#define STUDIOSLAB_GRAPHICSMODULE_H

#include "Core/Backend/Modules/Module.h"

namespace Slab::Graphics {

    class SlabGraphicsModule : public Core::GraphicsModule {
    public:
        explicit SlabGraphicsModule();

        void beginRender() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_GRAPHICSMODULE_H
