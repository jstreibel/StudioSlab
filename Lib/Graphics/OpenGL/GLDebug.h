//
// Created by joao on 2/09/23.
//

#ifndef STUDIOSLAB_GLDEBUG_H
#define STUDIOSLAB_GLDEBUG_H

#include <GL/glew.h>
#include <GL/gl.h>

namespace OpenGL {
    void OpenGL_Debug_Callback(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

    void StartupDebugLogging();
}

#endif //STUDIOSLAB_GLDEBUG_H
