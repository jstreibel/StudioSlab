//
// Created by joao on 08/04/2021.
//

#ifndef OPENGLTUTORIAL_VERTEXBUFFER_H
#define OPENGLTUTORIAL_VERTEXBUFFER_H


#include <GL/glew.h>
#include <vector>
#include "Drawable.h"

class VertexBuffer : public Drawable {
public:
    VertexBuffer();

    void setData(const std::vector<GLfloat> &data, GLenum primitive, GLuint dim);

    void draw(const int n);

    void draw() override;

private:
    GLenum primitive;
    GLuint dim;
    int count;
    void _bind();
    GLuint mVertexBufferID;
};


#endif //OPENGLTUTORIAL_VERTEXBUFFER_H
