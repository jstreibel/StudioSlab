//
// Created by joao on 19/09/23.
//

#ifndef STUDIOSLAB_MODERNGLTESTS_H
#define STUDIOSLAB_MODERNGLTESTS_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/OpenGL/VertexBuffer.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/Utils/Writer.h"
#include "Graphics/OpenGL/Texture2D_Color.h"
#include "Graphics/OpenGL/Texture2D_Real.h"

namespace Tests {

    using namespace Slab;

    class ModernGLTests : public Graphics::WindowRow {
        Graphics::OpenGL::Shader program;
        Graphics::OpenGL::VertexBuffer buffer;
        Graphics::OpenGL::Texture2D_Color texture;
        Graphics::OpenGL::Texture2D_Real realTexture;
        Graphics::Writer writer;

        ftgl::mat4 model{}, view{}, projection{};
    public:
        ModernGLTests();

        void draw() override;

    };

} // Tests

#endif //STUDIOSLAB_MODERNGLTESTS_H
