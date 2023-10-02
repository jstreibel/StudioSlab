//
// Created by joao on 29/04/2021.
//

// Taken from https://stackoverflow.com/questions/27027602/glvertexattribpointer-gl-invalid-operation-invalid-vao-vbo-pointer-usage

#ifndef OPENGLSTUDIES_MESH_H
#define OPENGLSTUDIES_MESH_H

#include "Graphics/OpenGL/OpenGL.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>
#include "Drawable.h"

struct Vertex{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
};

class Mesh : public Drawable {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint VAO;

    void initializeVAO();
    void draw() override;
};


#endif //OPENGLSTUDIES_MESH_H
