//
// Created by joao on 29/09/23.
//

#include "ModernOpenGLModule.h"
#include "Core/Tools/Log.h"

#include <GL/glew.h>

namespace Slab::Core {
    ModernOpenGLModule::ModernOpenGLModule() : Module("Modern OpenGL") {
        GLenum glewInitStatus = glewInit();
        if (glewInitStatus != GLEW_OK){
            Log::Error() << "Failed GLEW initialization: " << glewGetErrorString(glewInitStatus) << Log::Flush;
            throw Exception("Failed GLEW initialization");
        }
        Log::Success() << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << Log::Flush;
    }
} // Core