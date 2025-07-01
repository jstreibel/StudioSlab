//
// Created by joao on 6/30/25.
//

#ifndef GL_STATE_GUARD_H
#define GL_STATE_GUARD_H

#include "Utils/Arrays.h"

namespace Slab::Graphics::OpenGL {

    using GLint = int32_t;

    struct FGLStateGuard {
        // non-copyable
        FGLStateGuard(const FGLStateGuard&) = delete;
        FGLStateGuard& operator=(const FGLStateGuard&) = delete;

        GLuint PrevProgram;
        GLuint PrevVAO;
        GLuint PrevVBO;
        GLenum PrevActiveTex;

        struct TexState { GLenum Unit; GLuint Tex; };
        std::vector<TexState> TexStates;

        FGLStateGuard(GLint MaxTexUnitsToTrack = 8);

        ~FGLStateGuard();
    };

} // Slab::Graphics::OpenGL

#endif //GL_STATE_GUARD_H
