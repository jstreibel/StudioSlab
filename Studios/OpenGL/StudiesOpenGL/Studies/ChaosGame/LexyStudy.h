//
// Created by joao on 08/04/2021.
//

#ifndef OPENGLTUTORIAL_LEXYSTUDY_H
#define OPENGLTUTORIAL_LEXYSTUDY_H

#include <GL/glew.h>
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Program.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Drawables/VertexBuffer.h"
#include "Studios/OpenGL/StudiesOpenGL/Studies/StudiesBase.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/ZoomPanRotate.h"
#include "Studios/OpenGL/StudiesOpenGL/util/Clock.h"
#include "Studios/OpenGL/StudiesOpenGL/util/Vector.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Drawables/PointSet.h"
#include "ChaosGameSet.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Drawables/Plane.h"

#include <nanogui/window.h>

#include <string>
#include <vector>

class LexyStudy : public StudiesBase {
    nanogui::Screen &screen;
    nanogui::Window *window = nullptr;
public:
    LexyStudy(nanogui::Screen &screen);

    void doYourThing() override;

//    void resize(GLuint newWindowWidth, GLuint newWindowHeight);

private:
    Clock clock;
    const int N;
    int nDraw = 100;

    Program pointsShader;

    float bias = .5f;
    ChaosGame chaosGame;

    int winWidth, winHeight;
};


#endif //OPENGLTUTORIAL_LEXYSTUDY_H
