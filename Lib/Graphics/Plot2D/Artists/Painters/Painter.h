//
// Created by joao on 7/25/24.
//

#ifndef STUDIOSLAB_PAINTER_H
#define STUDIOSLAB_PAINTER_H

#include "Graphics/OpenGL/Shader.h"

namespace Slab::Graphics {

    class Painter : protected OpenGL::FShader {
    public:
        Painter(const Str &vertFilename, const Str &fragFilename);

        virtual void drawGUI();

        void Use() const override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_PAINTER_H
