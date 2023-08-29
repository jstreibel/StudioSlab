//
// Created by joao on 29/04/2021.
//

#include "Core/Graphics/OpenGL/OpenGL.h"

#include "Plane.h"

Plane::Plane() {
    std::vector<VertexType> verts = { {1, 1}, {-1, 1}, {-1, -1}, {1, -1}, {1, 1}};
    this->verts = verts;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(VertexType), &verts[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexType), 0);

    glBindVertexArray(0);
    glDeleteBuffers(1, &vertexBuffer);
}

void Plane::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, verts.size());
    glBindVertexArray(0);
}

