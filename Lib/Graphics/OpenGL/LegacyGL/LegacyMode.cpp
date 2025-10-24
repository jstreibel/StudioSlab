//
// Created by joao on 8/13/25.
//

#include "LegacyMode.h"

#include "Core/Backend/BackendManager.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/Utils.h"

namespace Slab::Graphics::OpenGL::Legacy {
    FShaderGuard::FShaderGuard()
    {
        if (IsInLegacyMode()) return;

        if(!Core::BackendManager::IsModuleLoaded("ModernOpenGL")) return; // Already in legacy mode

        GLint CurrentProgram;;
        glGetIntegerv(GL_CURRENT_PROGRAM, &CurrentProgram);

        if(CurrentProgram == 0) return; // so we don't lose LastProgramUsed

        LastProgramUsed = CurrentProgram;
        glUseProgram(0);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));
    }

    FShaderGuard::~FShaderGuard()
    {
        if(!Core::BackendManager::IsModuleLoaded("ModernOpenGL")) return;

        glUseProgram(LastProgramUsed);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + ":" + ToStr(__LINE__));
    }

    bool FShaderGuard::IsInLegacyMode()
    {
        GLint CurrentProgram;;
        glGetIntegerv(GL_CURRENT_PROGRAM, &CurrentProgram);

        return CurrentProgram == 0;
    }

    void SetupLegacyGL() {
        Texture::deactivate();
        FShader::LegacyGL();
    }
} // Slab::Graphics::OpenGL
