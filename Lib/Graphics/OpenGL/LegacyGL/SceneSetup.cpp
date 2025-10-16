//
// Created by joao on 6/8/24.
//

#include "Graphics/OpenGL/OpenGL.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/Types2D.h"
#include "SceneSetup.h"

#include "Core/Backend/BackendManager.h"
#include "Graphics/OpenGL/Utils.h"


namespace Slab::Graphics::OpenGL::Legacy {

    static GLint LastProgramUsed = 0;

    bool IsInLegacyMode()
    {
        GLint CurrentProgram;;
        glGetIntegerv(GL_CURRENT_PROGRAM, &CurrentProgram);

        return CurrentProgram == 0;
    }

    void PushLegacyMode()
    {
        if (IsInLegacyMode()) return;

        if(!Core::BackendManager::IsModuleLoaded("ModernOpenGL")) return;

        GLint CurrentProgram;;
        glGetIntegerv(GL_CURRENT_PROGRAM, &CurrentProgram);

        if(CurrentProgram == 0) return; // so we don't lose LastProgramUsed

        LastProgramUsed = CurrentProgram;
        glUseProgram(0);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));
    }

    void RestoreFromLegacyMode()
    {
        if(!Core::BackendManager::IsModuleLoaded("ModernOpenGL")) return;

        glUseProgram(LastProgramUsed);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));
    }


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

    void ResetModelView() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    }

    void SetupOrtho(const RectR &region, DevFloat zNear, DevFloat zFar) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(region.xMin, region.xMax, region.yMin, region.yMax, zNear, zFar);
    }

    void SetupOrthoI(const RectI &region, DevFloat zNear, DevFloat zFar) {
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
