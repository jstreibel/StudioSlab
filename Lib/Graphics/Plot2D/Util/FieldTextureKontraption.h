//
// Created by joao on 6/30/24.
//

#ifndef STUDIOSLAB_FIELDTEXTUREKONTRAPTION_H
#define STUDIOSLAB_FIELDTEXTUREKONTRAPTION_H

#include <glm/mat3x3.hpp>
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Texture2D_Real.h"
#include "Graphics/Types2D.h"

namespace Slab::Graphics {

    using FieldDataTexturePtr = Slab::Pointer<Graphics::OpenGL::Texture2D_Real>;

    struct FieldTextureThingy {
        FieldTextureThingy(int x_res, int y_res, RectR sub_region);

        FieldDataTexturePtr texture = nullptr;
        Pointer<Graphics::OpenGL::FVertexBuffer> vertexBuffer = nullptr;
    };

    struct FieldTextureKontraption {
        Vector <Pointer<FieldTextureThingy>> blocks={};
        Resolution n=0, m=0;
        Resolution xres=0, yres=0;

        FieldTextureKontraption() = default;
        FieldTextureKontraption(Resolution full_xres, Resolution full_yres, RectR region);

        void setValue(int i, int j, DevFloat value);
        Pointer <FieldTextureThingy> getBlock(int i, int j);

        bool upload(Index begin, CountType count);

        auto get_xres() const -> Resolution;
        auto get_yres() const -> Resolution;
    };

}
#endif //STUDIOSLAB_FIELDTEXTUREKONTRAPTION_H
