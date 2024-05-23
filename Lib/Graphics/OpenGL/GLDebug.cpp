//
// Created by joao on 2/09/23.
//

#include "GLDebug.h"
#include "Core/Tools/Log.h"

namespace Slab::Graphics::OpenGL {

    using namespace Core;

    void OpenGL_Debug_Callback(GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei length,
                               const GLchar *message,
                               const void *userParam){
        Log::Error() << "GL" << Log::Flush;
    }

    void StartupDebugLogging() {
        glDebugMessageCallback(OpenGL_Debug_Callback, nullptr);
        Log::Info() << "Registered OpenGL debug callback" << Log::Flush;
    }
}