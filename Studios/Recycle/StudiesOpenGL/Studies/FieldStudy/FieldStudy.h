//
// Created by joao on 08/04/2021.
//

#ifndef OPENGLTUTORIAL_FIELDSTUDY_H
#define OPENGLTUTORIAL_FIELDSTUDY_H

#include <GL/glew.h>
#include "Studios/OpenGL/StudiesOpenGL/Studies/StudiesBase.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/ZoomPanRotate.h"
#include "Studios/OpenGL/StudiesOpenGL/util/Clock.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Program.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Drawables/VertexBuffer.h"
#include "Studios/OpenGL/StudiesOpenGL/Studies/StudiesBase.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/ZoomPanRotate.h"

#include <oglplus/program.hpp>
#include <oglplus/vertex_array.hpp>
#include <oglplus/buffer.hpp>
#include <oglplus/uniform.hpp>

#include <string>
#include <vector>
#include <chrono>


class FieldStudy : public StudiesBase {
public:
    FieldStudy(int winWidth, int winHeight);

    void doYourThing() override;

private:
    Clock clock;
    //Viewport posSpaceVP, fourierSpaceVP;

    float phase = 0;

    float xRot = 0, yRot = 0;

    //oglplus::Program lineShader;
    //oglplus::VertexArray line;
    //Vector<GLfloat> lineData_v;
    //oglplus::Buffer lineData;

    oglplus::Uniform<GLfloat> uYPos, uXRot, uYRot, uPhase;
    oglplus::Uniform<GLboolean> uVertical;

    Program pendulumPhaseSpaceShader;
    VertexBuffer quadVertices;

    Program lineShader;
    VertexBuffer line;

    unsigned int winWidth, winHeight;
};


#endif //OPENGLTUTORIAL_FIELDSTUDY_H
