//
// Created by joao on 10/13/24.
//

#ifndef STUDIOSLAB_SLABGRAPHICS_H
#define STUDIOSLAB_SLABGRAPHICS_H

#include "Backend/GraphicBackend.h"

namespace Slab::Graphics {
    void Startup();
    void Finish();

    Pointer<GraphicBackend> GetGraphicsBackend();
}

#endif //STUDIOSLAB_SLABGRAPHICS_H
