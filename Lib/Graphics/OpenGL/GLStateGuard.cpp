//
// Created by joao on 6/30/25.
//

#include "OpenGL.h"
#include "GLStateGuard.h"

namespace Slab::Graphics::OpenGL {
    FGLStateGuard::FGLStateGuard(GLint MaxTexUnitsToTrack)
    {
        // Save core states
        {
            GLint Temp;
            glGetIntegerv(GL_CURRENT_PROGRAM, &Temp);
            PrevProgram = static_cast<GLuint>(Temp);
        }
        {
            GLint Temp;
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &Temp);
            PrevVAO = static_cast<GLuint>(Temp);
        }
        {
            GLint Temp;
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &Temp);
            PrevVBO = static_cast<GLuint>(Temp);
        }
        {
            GLint Temp;
            glGetIntegerv(GL_ACTIVE_TEXTURE, &Temp);
            PrevActiveTex = static_cast<GLenum>(Temp);
        }

        // Clamp to hardware limit
        GLint MaxUnits = 0;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxUnits);
        GLint Count = std::min(MaxUnits, MaxTexUnitsToTrack);

        // Capture each unit’s bound 2D texture
        TexStates.reserve(Count);
        for (GLint i = 0; i < Count; ++i) {
            GLenum Unit = static_cast<GLenum>(GL_TEXTURE0 + i);
            glActiveTexture(Unit);
            GLint Bound = 0;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &Bound);
            TexStates.push_back({ Unit, static_cast<GLuint>(Bound) });
        }
        // restore
        glActiveTexture(PrevActiveTex);
    }

    FGLStateGuard::~FGLStateGuard()
    {
        // restore program/buffers
        glUseProgram(PrevProgram);
        glBindVertexArray(PrevVAO);
        glBindBuffer(GL_ARRAY_BUFFER, PrevVBO);

        // restore textures
        for (auto const& s : TexStates) {
            glActiveTexture(s.Unit);
            glBindTexture(GL_TEXTURE_2D, s.Tex);
        }
        glActiveTexture(PrevActiveTex);
    }
} // Slab::Graphics::OpenGL