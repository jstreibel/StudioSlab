//
// Created by joao on 18/10/2019.
//

#include <iostream>
#include "ZoomPanRotate.h"

ZoomPanRotate::ZoomPanRotate() = default;

void ZoomPanRotate::zprReshape(int vpX_, int vpY_, int vpW_,int vpH_) {
    vpW = vpW_;
    vpH = vpH_;

}

void ZoomPanRotate::zprMouseButton(int button, int state, int x, int y) {
    _mouseX = x;
    _mouseY = y;

    if (state==GLUT_UP)
        switch (button)
        {
            case GLUT_LEFT_BUTTON:   _mouseLeft   = false; break;
            case GLUT_MIDDLE_BUTTON: _mouseMiddle = false; break;
            case GLUT_RIGHT_BUTTON:  _mouseRight  = false; break;
            default: break;
        }
    else
        switch (button)
        {
            case GLUT_LEFT_BUTTON:   _mouseLeft   = true; break;
            case GLUT_MIDDLE_BUTTON: _mouseMiddle = true; break;
            case GLUT_RIGHT_BUTTON:  _mouseRight  = true; break;
            default: break;
        }
}

void ZoomPanRotate::zprMouseMotion(int x, int y) {
    bool changed = false;

    const int dx = x - _mouseX;
    const int dy = y - _mouseY;

    if (dx==0 && dy==0)
        return;

    if (_mouseMiddle || (_mouseLeft && _mouseRight))
    {
        scale *= 1.00 +  (double)dy*0.01;
    }
    else if (_mouseLeft)
    {
        xAngle += dy / (double) (vpW + 1) * 2;
        zAngle += dx / (double) (vpH + 1) * 2;
    }
    else if (_mouseRight)
    {
        yScale += dy / (double) (vpW + 1) * 2;
    }

    _mouseX = x;
    _mouseY = y;
}

const float *ZoomPanRotate::getModelview() {
    modelview = glm::mat4x4(1,0,0,0,
                            0,1,0,0,
                            0,0,1,0,
                            0,0,0,1);
    modelview = glm::scale(modelview, {scale, (vpW/vpH)*scale, 0.1*scale});
    modelview = glm::rotate(modelview, float(xAngle), {1., 0., 0.});
    modelview = glm::rotate(modelview, float(zAngle), {0., 0., 1.});
    modelview = glm::scale(modelview, {1., 1., yScale});

    return (const float*)glm::value_ptr(modelview);
}
