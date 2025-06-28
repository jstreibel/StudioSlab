//
// Created by joao on 6/8/24.
//

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/Types2D.h"
#include "SceneSetup.h"


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

    void ResetModelview() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void SetupOrtho(const RectR &region, DevFloat zNear, DevFloat zFar) {
        OpenGL::Shader::remove();

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(region.xMin, region.xMax, region.yMin, region.yMax, zNear, zFar);
    }

    void PopScene() {
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }


}