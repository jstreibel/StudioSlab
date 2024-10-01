//
// Created by joao on 9/30/24.
//

#include "CairoInterface.h"
#include "Texture2D_Color.h"

namespace Slab::Graphics::OpenGL {

    Pointer<Texture2D>
    BuildTexture2DFromCairoSurface(Cairo::RefPtr<const Cairo::ImageSurface> surface) {
        if(surface->get_format() != Cairo::FORMAT_ARGB32) NOT_IMPLEMENTED;

        auto w = surface->get_width();
        auto h = surface->get_height();
        auto data = surface->get_data();

        auto texture = New<Texture2D_Color>(w, h);
        constexpr auto size = sizeof(int);

        for(int i=0; i<w; ++i) {
            for(int j=0; j<h; ++j) {
                fix k = (i+j*w)*size;

                auto color = &data[k];
                auto a = float(color[0])/255.0f;
                auto r = float(color[1])/255.0f;
                auto g = float(color[2])/255.0f;
                auto b = float(color[3])/255.0f;

                texture->setColor(i, j, Color(r, g, b, a));
            }
        }

        texture->upload();

        return texture;
    }

}