//
// Created by joao on 29/09/23.
//

#include "ModernOpenGLModule.h"
#include "Core/Tools/Log.h"

#include <GL/glew.h>

namespace Slab::Graphics {
    ModernOpenGLModule::ModernOpenGLModule() : Module("Modern OpenGL", false) {
        GLenum glewInitStatus = glewInit();
        if (glewInitStatus != GLEW_OK){
            Core::Log::Error() << "Failed GLEW initialization: " << glewGetErrorString(glewInitStatus) << Core::Log::Flush;
            throw Exception("Failed GLEW initialization");
        }
        Core::Log::Success() << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << Core::Log::Flush;
    }
} // Core