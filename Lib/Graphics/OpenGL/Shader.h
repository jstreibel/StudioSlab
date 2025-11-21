//
// Created by joao on 16/09/23.
//

#ifndef STUDIOSLAB_SHADER_H
#define STUDIOSLAB_SHADER_H

#include "Utils/Types.h"
#include "Math/Formalism/Categories.h"
#include "Texture.h"

#include "OpenGL.h"

#include <glm/glm.hpp>
// #include <glm/detail/type_mat4x4.hpp>

#include <map>

namespace Slab::Graphics::OpenGL {

    using namespace Math;

    enum ShaderType {
        VertexShader = GL_VERTEX_SHADER,
        FragmentShader = GL_FRAGMENT_SHADER
    };

    class FShader {
        GLuint Handle;
        std::map<GLuint, std::shared_ptr<FTexture>> TextureUnits;
        Str VertFileName;
        Str FragFileName;

    public:
        virtual ~FShader() = default;

        FShader(const Str& vertFilename, const Str& fragFilename);

        void BindTextures() const;

        virtual void Use() const;
        static void LegacyGL();

        auto GetHandle() const -> GLuint {return Handle; };

        void SetUniform(const Str& Name, GLint Value) const;
        void SetUniform(const Str& Name, GLfloat Value) const;
        void SetUniform(const Str& Name, const Real2D& Vec2) const;
        void SetUniform(const Str& Name, const glm::vec3& Vec3) const;
        void SetUniform(const Str& Name, const glm::vec4& Vec4) const;
        void SetUniform(const Str& Name, const glm::mat3& Mat3) const;
        void SetUniform(const Str& Name, const glm::mat4& Mat4) const;
        void SetUniform(const Str& Name, FTexture& texture);

        void SetUniform(const Str& Name, std::array<float, 3> Vec3) const;
        void SetUniform(const Str& Name, std::array<float, 4> Vec4) const;
        void SetUniform3x3(const Str& Name, const float* Mat3) const;
        void SetUniform4x4(const Str& Name, const float* Mat4) const;

    };

} // OpenGL

#endif //STUDIOSLAB_SHADER_H
