//
// Created by joao on 28/05/24.
//

#ifndef STUDIOSLAB_SCENESETUP_H
#define STUDIOSLAB_SCENESETUP_H

#include "Graphics/OpenGL/OpenGL.h"

namespace Slab::Graphics::OpenGL::Legacy {

    void PushScene() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_DEPTH_TEST);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
    }

    void PopScene() {
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
}

#endif //STUDIOSLAB_SCENESETUP_H
