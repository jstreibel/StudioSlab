//
// Created by joao on 29/04/2021.
//

#ifndef OPENGLSTUDIES_POINTSET_H
#define OPENGLSTUDIES_POINTSET_H


#include "Drawable.h"

#include "Graphics/OpenGL/OpenGL.h"
#include <glm/vec3.hpp>
#include <vector>

class PointSet : public Drawable {
    std::vector<glm::vec3> pointSet;
    GLuint VAO; GLboolean initialized = false;
    GLuint begin, end;
public:
    GLuint getBegin() const;

    void setBegin(GLuint begin);

    GLuint getEnd() const;

    void setEnd(GLuint anEnd);

public:
    void draw() override;
    void initialize(const std::vector<glm::vec3> &points);
    unsigned getSize() const;
    const std::vector<glm::vec3> &getPointSet();
};


#endif //OPENGLSTUDIES_POINTSET_H
