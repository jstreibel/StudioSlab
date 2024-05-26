//
// Created by joao on 29/04/2021.
//

#ifndef OPENGLSTUDIES_PLANE_H
#define OPENGLSTUDIES_PLANE_H

#include "Drawable.h"
#include <vector>
#include "Graphics/OpenGL/OpenGL.h"
#include <glm/vec2.hpp>

class Plane : public Drawable {
    typedef glm::vec2 VertexType;
    Vector<VertexType> verts;
    GLuint VAO;
public:
    Plane();

    void draw() override;
};


#endif //OPENGLSTUDIES_PLANE_H
