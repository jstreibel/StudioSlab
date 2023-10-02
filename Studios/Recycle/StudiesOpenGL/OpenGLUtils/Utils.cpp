//
// Created by joao on 08/04/2021.
//

#include "Utils.h"
#include "Graphics/OpenGL/OpenGL.h"

void checkGLErrors(const std::string msg = "") {
    GLenum error;

    while (error = glGetError()){// ==GL_NO_ERROR
            std::cout << "GL error " << error << ", message: " << msg << std::endl;
    }
}