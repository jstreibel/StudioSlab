//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_SHADERLOADER_H
#define STUDIOSLAB_SHADERLOADER_H

#include "Shader.h"

namespace Graphics::OpenGL {

    class ShaderLoader {
    public:
        static GLuint Load(const Str& vertFilename, const Str& fragFilename);

        static GLuint Compile(const Str &source, ShaderType);
    };

} // Graphics::OpenGL

#endif //STUDIOSLAB_SHADERLOADER_H
