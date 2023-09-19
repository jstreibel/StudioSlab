//
// Created by joao on 19/09/23.
//

#include "ModernGLTests.h"

#include "Utils/Resources.h"
#include "Core/Graphics/OpenGL/OpenGL.h"

struct TestVertex {
    float x, y, z;
    float s, t;
    float r, g, b, a;
};

fix texDim = 256;

namespace Tests {
    ModernGLTests::ModernGLTests()
    : program(Resources::ShadersFolder + "v3f-t2f-c4f.vert", Resources::ShadersFolder + "v3f-t2f-c4f.frag")
    , buffer("vertex:3f,tex_coord:2f,color:4f")
    , texture(texDim, texDim)
    , writer(Resources::fontFileName(4), 100)
    {
        addWindow(std::make_shared<Window>());

        GLuint indices[6] = {0, 1, 2, 0, 2, 3};

        float r=.5, g=.5, b=1, a=1;
        TestVertex square[4] = {
            {-0.75f, -0.75f, 0.0f,   0.0f, 0.0f,    r, g, b, a},
            {+0.75f, -0.75f, 0.0f,   1.0f, 0.0f,    r, g, b, a},
            {+0.75f, +0.75f, 0.0f,   1.0f, 1.0f,    r, g, b, a},
            {-0.75f, +0.75f, 0.0f,   0.0f, 1.0f,    r, g, b, a},
        };

        buffer.pushBack(square, 4, indices, 6);

        mat4_set_identity(&projection);
        mat4_set_identity(&model);
        mat4_set_identity(&view);

        for(auto i=0; i<texDim; ++i){
            for(auto j=0; j<texDim; ++j){
                fix color = Styles::Color(1, 1, 1, 1);
                texture.setColor(i, j, color);
            }
        }

        texture.upload();
    }

    void ModernGLTests::draw() {
        WindowRow::draw();

        {
            texture.bind();
            program.use();
            program.setUniform("texture", 0);
            program.setUniform4x4("model", model.data);
            program.setUniform4x4("view", view.data);
            program.setUniform4x4("projection", projection.data);

            buffer.render(GL_TRIANGLES);
        }

        {
            fix vp = getViewport();
            writer.reshape(vp.width(), vp.height());
            writer.write("VertexBuffer tests", {300, (double) geth() - 300});
        }
    }
} // Tests