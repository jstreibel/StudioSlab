//
// Created by joao on 19/09/23.
//

#include "ModernGLTests.h"

#include "Core/Tools/Resources.h"
#include "Graphics/OpenGL/OpenGL.h"

struct TestVertex {
    float x, y;
    float s, t;
};

fix texDim = 4096;

namespace Tests {
    using namespace Slab;

    ModernGLTests::ModernGLTests()
    : Graphics::WindowRow("Modern OpenGL test")
    , program(Core::Resources::ShadersFolder + "tests.vert", Core::Resources::ShadersFolder + "tests.frag")
    , buffer("vertex:2f,tex_coord:2f")
    , texture(texDim, texDim)
    , realTexture(texDim, texDim)
    , writer(Core::Resources::fontFileName(4), 80)
    {
        addWindow(std::make_shared<SlabWindow>());

        GLuint indices[6] = {0, 1, 2, 0, 2, 3};

        fix tMin = -0.1f;
        fix tMax =  1.1f;

        TestVertex square[4] = {
            {-0.65f, -0.65f,   tMin, tMin},
            {+0.65f, -0.65f,   tMax, tMin},
            {+0.65f, +0.65f,   tMax, tMax},
            {-0.65f, +0.65f,   tMin, tMax},
        };

        buffer.pushBack(square, 4, indices, 6);

        for(auto i=0; i<texDim; ++i){
            fix x = i/(float)texDim;
            for(auto j=0; j<texDim; ++j){
                fix y = j/(float)texDim;
                fix r = sqrt(x*x + y*y);
                fix color = Graphics::Color(.5f + .5f*sinf(2*M_PI*r), .5f + .5f*cosf(2*M_PI*r), 1);
                texture.setColor(i, j, color);
                realTexture.setValue(i, j, .5f+.5f*cosf(8*M_PI*r));
            }
        }

        texture.upload();
        realTexture.upload();

        program.setUniform("transformMatrix", {1,0,0,
                                               0,1,0,
                                               0,0,1});
    }

    void ModernGLTests::draw() {
        WindowRow::draw();

        {
            // texture.bind();
            realTexture.bind();
            program.use();
            program.setUniform("texture", 0);

            buffer.render(GL_TRIANGLES);
        }

        {
            fix vp = getViewport();
            writer.reshape(vp.width(), vp.height());
            writer.write("VertexBuffer + Texture tests", {200, (double) GetHeight() - 200});
        }
    }
} // Tests