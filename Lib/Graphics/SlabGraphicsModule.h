//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_SLABGRAPHICSMODULE_H
#define STUDIOSLAB_SLABGRAPHICSMODULE_H

#include "Graphics/Modules/GraphicsModule.h"

namespace Slab::Graphics {

    class SlabGraphicsModule : public GraphicsModule {
    public:
        explicit SlabGraphicsModule();

        void beginRender() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_SLABGRAPHICSMODULE_H
