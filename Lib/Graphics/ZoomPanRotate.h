//
// Created by joao on 18/10/2019.
//

#ifndef V_SHAPE_ZOOMPANROTATE_H
#define V_SHAPE_ZOOMPANROTATE_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"

#include "Graphics/OpenGL/OpenGL.h"
#include <GL/glut.h>
#include <glm/mat4x4.hpp>

class ZoomPanRotate : public Core::GUIEventListener {
public:
    ZoomPanRotate();

    void zprReshape(int vpX, int vpY, int vpW,int vpH);
    void zprMouseButton(int button, int state, int x, int y);
    void zprMouseMotion(int x, int y);

    const float* getModelview();

private:
    Real xAngle =.0, zAngle =.0, scale=1., yScale=1.;

    /* ORTHO PROJECTION */
    Real _left   = 0.0;

    int  _mouseX      = 0;
    int  _mouseY      = 0;

    bool _mouseLeft   = false;
    bool _mouseMiddle = false;
    bool _mouseRight  = false;

    GLint vpW=100, vpH=100;

    glm::mat4x4 modelview = glm::mat4x4(1,0,0,0,
                                        0,1,0,0,
                                        0,0,1,0,
                                        0,0,0,1);

};


#endif //V_SHAPE_ZOOMPANROTATE_H
