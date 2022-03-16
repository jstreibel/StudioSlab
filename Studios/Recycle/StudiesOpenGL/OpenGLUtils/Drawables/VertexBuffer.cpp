//
// Created by joao on 08/04/2021.
//

#include "VertexBuffer.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Utils.h"
#include "oglplus/all.hpp"

VertexBuffer::VertexBuffer() {
    //oglplus::
    glGenBuffers(1, &mVertexBufferID);
}


void VertexBuffer::setData(const std::vector<GLfloat> &vertData, GLenum primitive, GLuint dim) {
    // TODO: typedef std::vector<GLfloat> pra FloatData, em um arquivo dataTypes.h e cpp
    _bind();

    this->dim = dim;
    this->count = vertData.size();
    this->primitive = primitive;

    if(count%dim) std::cout << "Warning: exceeding data, count%dim = " << count%dim;

    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*count, &vertData[0], GL_DYNAMIC_DRAW);
}


void VertexBuffer::draw(int n) {
    if(n < 0) n = count;
    if(n > count) n = count;

    // DRAW STUFF
    // 1st attribute buffer : quadVertices
    glEnableVertexAttribArray(0);
    _bind();

    glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the pendulumPhaseSpaceShader.
            dim,                // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    glDrawArrays(primitive, 0, n/dim);
    glDisableVertexAttribArray(0);
}

void VertexBuffer::_bind() {
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferID);
}

void VertexBuffer::draw() {
    throw "Not implemented.";
}
