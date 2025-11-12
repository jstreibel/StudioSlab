//
// Created by joao on 9/30/24.
//

#ifndef STUDIOSLAB_CAIROINTERFACE_H
#define STUDIOSLAB_CAIROINTERFACE_H

#include <cairomm/surface.h>
#include "Texture2D.h"

namespace Slab::Graphics::OpenGL {

    TPointer<FTexture2D>
    BuildTexture2DFromCairoSurface(Cairo::RefPtr<const Cairo::ImageSurface>);

}

#endif //STUDIOSLAB_CAIROINTERFACE_H
