//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_SLABGRAPHICS_H
#define STUDIOSLAB_SLABGRAPHICS_H

#include "Backend/GraphicBackend.h"

namespace Slab::Graphics {
    void Register();
    void Finish();

    Core::GraphicBackend& GetGraphicsBackend();
}

#endif //STUDIOSLAB_SLABGRAPHICS_H
