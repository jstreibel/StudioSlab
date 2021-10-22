//
// Created by joao on 20/04/2021.
//

#ifndef OPENGLSTUDIES_OGLPLUSSTUDY_H
#define OPENGLSTUDIES_OGLPLUSSTUDY_H

#include <GL/glew.h>

#include <oglplus/context.hpp>
#include <oglplus/shader.hpp>
#include <oglplus/program.hpp>
#include <oglplus/vertex_array.hpp>
#include <oglplus/buffer.hpp>

#include <oglplus/all.hpp>

#include "StudiesBase.h"


class OGLplusStudy : public StudiesBase {
    oglplus::Context gl;
    oglplus::VertexShader vs;
    oglplus::FragmentShader fs;
    oglplus::Program prog;
    oglplus::VertexArray triangle;

    oglplus::Array<oglplus::Buffer> buffs;

    // VBOs for the triangle's vertices and colors
    oglplus::Buffer verts;
    oglplus::Buffer colors;

public:
    OGLplusStudy();
    void doYourThing() override;
};


#endif //OPENGLSTUDIES_OGLPLUSSTUDY_H
