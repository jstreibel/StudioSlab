//
// Created by joao on 9/30/24.
//

#ifndef STUDIOSLAB_TEXTUREBUILDER_H
#define STUDIOSLAB_TEXTUREBUILDER_H

#include <cairomm/surface.h>
#include "Texture2D.h"

namespace Slab::Graphics::OpenGL {



    Pointer<Texture2D>
    BuildTexture2DFromCairoSurface(Cairo::RefPtr<const Cairo::ImageSurface>);

}

#endif //STUDIOSLAB_TEXTUREBUILDER_H
