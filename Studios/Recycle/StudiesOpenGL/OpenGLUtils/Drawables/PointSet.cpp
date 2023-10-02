//
// Created by joao on 29/04/2021.
//

#include "Graphics/OpenGL/OpenGL.h"
#include "PointSet.h"

void PointSet::initialize(const std::vector<glm::vec3> &points) {
    if(initialized){
        glDeleteVertexArrays(1, &VAO);
        initialized = false;
    }

    pointSet = points;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, points.size()*sizeof(glm::vec3), &pointSet[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    glBindVertexArray(0);
    glDeleteBuffers(1, &vertexBuffer);

    begin = 0;
    end = points.size() - 1;

    initialized = true;
}

void PointSet::draw() {
    if(!initialized) throw "PointSet not initialized";

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, begin, end);
    glBindVertexArray(0);

}

unsigned PointSet::getSize() const {
    return pointSet.size();
}

const std::vector<glm::vec3> &PointSet::getPointSet() {
    return pointSet;
}

GLuint PointSet::getBegin() const {
    return begin;
}

void PointSet::setBegin(GLuint begin) {
    if(begin >= pointSet.size()) return;

    PointSet::begin = begin;
}

GLuint PointSet::getEnd() const {
    return end;
}

void PointSet::setEnd(GLuint end) {
    if(end > pointSet.size()) return;

    PointSet::end = end;
}

