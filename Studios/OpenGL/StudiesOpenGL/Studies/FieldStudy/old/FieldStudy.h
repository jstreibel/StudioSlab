//
// Created by joao on 08/04/2021.
//

#ifndef OPENGLTUTORIAL_FIELDSTUDY_OLD_H
#define OPENGLTUTORIAL_FIELDSTUDY_OLD_H

#include <GL/glew.h>
#include "Studios/OpenGL/StudiesOpenGL/Studies/StudiesBase.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/ZoomPanRotate.h"
#include "../../OpenGLUtils/Viewport.h"
#include "Studios/OpenGL/StudiesOpenGL/util/Clock.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Program.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Drawables/VertexBuffer.h"

//#include <oglplus/program.hpp>
//#include <oglplus/vertex_array.hpp>

#include <string>
#include <vector>
#include <chrono>


class FieldStudy_old : public StudiesBase {
public:
    FieldStudy_old(int winWidth, int winHeight);

    void doYourThing(GLFWwindow &window) override;

private:
    Clock clock;
    Viewport posSpaceVP, fourierSpaceVP;

    float phase = 0;

    float xRot = 0, yRot = 0;

    Program lineShader;
    VertexBuffer line;

    unsigned int winWidth, winHeight;
};


#endif //OPENGLTUTORIAL_FIELDSTUDY_OLD_H
