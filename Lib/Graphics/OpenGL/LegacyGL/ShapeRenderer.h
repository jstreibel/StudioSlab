//
// Created by joao on 6/8/24.
//

#ifndef STUDIOSLAB_SHAPERENDERER_H
#define STUDIOSLAB_SHAPERENDERER_H

#include "Graphics/OpenGL/Texture.h"
#include "Graphics/Plot2D/Shapes/Shape.h"
#include "Graphics/Styles/Colors.h"
#include "Math/Formalism/Categories.h"

namespace Slab::Graphics::OpenGL::Legacy {
    void SetColor(const FColor&);

    void DrawRectangle(const FRectangleShape &);

    void DrawRectangleWithTexture(const FRectangleShape &, const FTexture&);

    void DrawLine(
        const Math::Real2D& start,
        const Math::Real2D& end,
        const FColor& color=White,
        const DevFloat &thickness=1.0f);

}

#endif //STUDIOSLAB_SHAPERENDERER_H
