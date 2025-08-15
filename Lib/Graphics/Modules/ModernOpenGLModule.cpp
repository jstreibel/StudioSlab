//
// Created by joao on 29/09/23.
//

#include "ModernOpenGLModule.h"
#include "Core/Tools/Log.h"
#include "Graphics/SlabGraphics.h"

#include <GL/glew.h>

namespace Slab::Graphics {
    ModernOpenGLModule::ModernOpenGLModule() : SlabModule("Modern OpenGL", false) {
        GetGraphicsBackend()->GetMainSystemWindow(); // Guarantee a OpenGL context is built.

        GLenum glewInitStatus = glewInit();
        if (glewInitStatus != GLEW_OK){
            Core::Log::Error() << "Failed GLEW initialization: "
                << Core::Log::BGRed << Core::Log::FGBlack << " " << glewGetErrorString(glewInitStatus) << " "
                << Core::Log::ResetFormatting << Core::Log::Flush;
            throw Exception("Failed GLEW initialization");
        }
        Core::Log::Info() << "Using GLEW " << glewGetString(GLEW_VERSION) << Core::Log::Flush;

        Core::Log::Info() << "Supported OpenGL version " << glGetString(GL_VERSION) << Core::Log::Flush;
    }
} // Core