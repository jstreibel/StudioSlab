//
// Created by joao on 29/04/2021.
//

#ifndef OPENGLSTUDIES_CHAOSGAMESET_H
#define OPENGLSTUDIES_CHAOSGAMESET_H


#include <vector>
#include <glm/vec2.hpp>
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Drawables/PointSet.h"

class ChaosGame {
    PointSet basePointsSet;
    PointSet chaosPointSet;
public:
    ChaosGame(float radius, int NSides);

    void draw(int n);

    void generatePointSet(glm::vec3 seed, int n, bool allowRepeat, float bias);

};


#endif //OPENGLSTUDIES_CHAOSGAMESET_H
