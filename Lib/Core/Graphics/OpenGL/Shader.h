//
// Created by joao on 16/09/23.
//

#ifndef STUDIOSLAB_SHADER_H
#define STUDIOSLAB_SHADER_H

#include <GL/gl.h>
#include <glm/detail/type_mat4x4.hpp>
#include "Utils/Types.h"

namespace OpenGL {

    class Shader {
        GLuint handle;

    public:
        Shader(const Str& vertFilename, const Str& fragFilename);

        void use() const;
        void remove() const;

        void setUniform(const Str& name, GLint value) const;
        void setUniform(const Str& name, const glm::mat4& mat4) const;
        void setUniform4x4(const Str& name, const float* mat4) const;
    };

} // OpenGL

#endif //STUDIOSLAB_SHADER_H
