//
// Created by joao on 07/04/2021.
//

#ifndef OPENGLTUTORIAL_PENDULUMSTUDY_H
#define OPENGLTUTORIAL_PENDULUMSTUDY_H

#include <GL/glew.h>
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Program.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Drawables/VertexBuffer.h"
#include "StudiesBase.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Drawables/Plane.h"

#include <SFML/Graphics.hpp>

#include <SFML/OpenGL.hpp>
#include <string>
#include <vector>

class PendulumStudy : public StudiesBase {
public:
    PendulumStudy();

    void doYourThing() override;
    void event(sf::Event &event);
    void resize(GLuint newWindowWidth, GLuint newWindowHeight);

private:
    Program shader;
    Plane canvas;

    unsigned int winWidth, winHeight;
};


#endif //OPENGLTUTORIAL_PENDULUMSTUDY_H
