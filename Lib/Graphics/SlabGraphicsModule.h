//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_SLABGRAPHICSMODULE_H
#define STUDIOSLAB_SLABGRAPHICSMODULE_H

#include "Graphics/Modules/GraphicsModule.h"

namespace Slab::Graphics {

    class FSlabGraphicsModule : public FGraphicsModule {
    public:
        explicit FSlabGraphicsModule();

    };

    using SlabGraphicsModule [[deprecated("Use FSlabGraphicsModule")]] = FSlabGraphicsModule;

} // Slab::Graphics

#endif //STUDIOSLAB_SLABGRAPHICSMODULE_H
