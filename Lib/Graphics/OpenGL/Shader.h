//
// Created by joao on 16/09/23.
//

#ifndef STUDIOSLAB_SHADER_H
#define STUDIOSLAB_SHADER_H

#include "Utils/Types.h"
#include "Math/Formalism/Categories.h"
#include "Texture.h"

#include <GL/gl.h>
#include <glm/detail/type_mat4x4.hpp>

#include <map>

namespace Graphics::OpenGL {

    enum ShaderType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER
    };

    class Shader {
        GLuint handle;
        std::map<GLuint, std::shared_ptr<Texture>> textureUnits;
        Str vertFileName;
        Str fragFileName;

    public:
        Shader(const Str& vertFilename, const Str& fragFilename);

        void bindTextures() const;

        void use() const;
        static void remove() ;

        auto getHandle() const -> GLuint {return handle; };

        void setUniform(const Str& name, GLint value) const;
        void setUniform(const Str& name, GLfloat value) const;
        void setUniform(const Str& name, Real2D vec2) const;
        void setUniform(const Str& name, const glm::vec3& vec3) const;
        void setUniform(const Str& name, const glm::vec4& vec4) const;
        void setUniform(const Str& name, const glm::mat3& mat3) const;
        void setUniform(const Str& name, const glm::mat4& mat4) const;
        void setUniform(const Str& name, Texture& texture);

        void setUniform(const Str& name, std::array<float, 3> vec3) const;
        void setUniform(const Str& name, std::array<float, 4> vec4) const;
        void setUniform3x3(const Str& name, const float* mat3) const;
        void setUniform4x4(const Str& name, const float* mat4) const;

    };

} // OpenGL

#endif //STUDIOSLAB_SHADER_H
