//
// Created by joao on 29/09/23.
//

#include "ModernOpenGLModule.h"
#include "Core/Tools/Log.h"
#include "Graphics/SlabGraphics.h"

#include <GL/glew.h>

namespace Slab::Graphics {
    FModernOpenGLModule::FModernOpenGLModule() : SlabModule("Modern OpenGL", false) {
        GetGraphicsBackend()->GetMainSystemWindow(); // Guarantee a OpenGL context is built.

        GLenum glewInitStatus = glewInit();
        if (glewInitStatus != GLEW_OK){
            Core::FLog::Error() << "Failed GLEW initialization: "
                << Core::FLog::BGRed << Core::FLog::FGBlack << " " << glewGetErrorString(glewInitStatus) << " "
                << Core::FLog::ResetFormatting << Core::FLog::Flush;
            throw Exception("Failed GLEW initialization");
        }
        Core::FLog::Info() << "Using GLEW " << glewGetString(GLEW_VERSION) << Core::FLog::Flush;

        Core::FLog::Info() << "Supported OpenGL version " << glGetString(GL_VERSION) << Core::FLog::Flush;
    }
} // Core
