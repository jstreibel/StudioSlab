//
// Created by joao on 3/31/23.
//

#include "Utils.h"

#include <GL/gl.h>

void OpenGLUtils::drawOrthoNormalized(Rect rect) {
    // TODO ultra-provisório
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRectd(rect.xMin, rect.yMin, rect.xMax, rect.yMax);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
