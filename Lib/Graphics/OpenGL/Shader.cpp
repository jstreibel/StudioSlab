//
// Created by joao on 16/09/23.
//

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Core/Tools/Log.h"
#include "Utils.h"
#include "Core/Backend/BackendManager.h"
#include "ShaderLoader.h"
#include "Core/SlabCore.h"

#include <array>
#include <ranges>

// #include "crude_json.h"

#define CHECK_UNIFORM_EXISTS \
    if(loc == -1){               \
    Log::Error("While setting uniform '") << Name << "'. Uniform not found, " \
        << "might have been pruned by shader compiler."; \
    return; \
    }

#define CHECK_UNIFORM_ERRORS \
    CheckGLErrors(Str("uniform '") + Name + "' @ " + __PRETTY_FUNCTION__);

namespace Slab::Graphics::OpenGL {

    using Log = Core::Log;

    void ListShaderUniforms(GLuint handle, std::basic_ostream<char, std::char_traits<char>> &log) {
        GLint numActiveUniforms = 0;
        GLint maxLength;

        glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

        // Get the maximum length of attribute names
        glGetProgramiv(handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);

        for (GLint i = 0; i < numActiveUniforms; ++i) {
            GLsizei length;
            GLint size;
            GLenum type;
            Vector<GLchar> Name(maxLength); // Variable to hold the uniform name

            glGetActiveUniform(handle, static_cast<GLuint>(i), maxLength, &length, &size, &type, &Name[0]);

            GLint location = glGetUniformLocation(handle, &Name[0]);

            log << "\n\t\t\t\t\t"
                << (i+1) << "/" << numActiveUniforms << " @ location " << location << " : \""
                << &Name[0] << "\" (" << GLTypeToGLSLType(type) << ")";
        }
    }

    void ListShaderAttributes(GLuint program, std::basic_ostream<char, std::char_traits<char>> &log) {
        GLint numAttributes;
        GLint maxLength;

        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

        for (GLint i = 0; i < numAttributes; ++i) {
            GLint size;
            GLenum type;
            GLsizei length;
            Vector<GLchar> Name(maxLength); // Variable to hold the uniform name

            // Get information about the i-th active attribute
            glGetActiveAttrib(program, i, maxLength, &length, &size, &type, &Name[0]);

            // Get the location of the attribute
            GLint location = glGetAttribLocation(program, &Name[0]);

            log << "\n\t\t\t\t\t"
                << (i+1) << "/" << numAttributes << " @ location " << location << " : \""
                << &Name[0] << "\" (" << GLTypeToGLSLType(type) << ")" ;
        }
    }




    FShader::FShader(const Str& vertFilename, const Str& fragFilename)
    : VertFileName(vertFilename)
    , FragFileName(fragFilename)
    {
        Core::LoadModule("ModernOpenGL");

        Handle = ShaderLoader::Load(vertFilename, fragFilename);

        Log::Note() << "Shader files '" << vertFilename
                                << "' and '" << fragFilename << "' loaded and compiled." << Log::Flush;
        auto &log = Log::Debug() << "Are active in this shader:";

        log << "\n\t\t\t\tUniforms:";
        ListShaderUniforms(Handle, log);

        log << "\n\t\t\t\tVertex attributes:";
        ListShaderAttributes(Handle, log);
        log << Log::Flush;
    }

    void FShader::BindTextures() const {
        for(const auto& TexturePtr : TextureUnits | std::views::values)
            TexturePtr->Bind();
    }

    void FShader::Use() const {
        BindTextures();

        glUseProgram(Handle);
    }

    void FShader::SetUniform(const Str& Name, GLint Value) const {
        this->Use();

        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform1i(loc, Value);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + Name + Log::FGBlue + "' = " + ToStr(Value));
    }

    void FShader::SetUniform(const Str& Name, GLfloat Value) const {
        this->Use();

        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform1f(loc, Value);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + Name + Log::FGBlue + "' = " + ToStr(Value));
    }

    void FShader::SetUniform(const Str &Name, const Real2D& Vec2) const {
        this->Use();

        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform2f(loc, static_cast<GLfloat>(Vec2.x), static_cast<GLfloat>(Vec2.y));

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + Name + Log::FGBlue + "' = " + ToStr(Vec2.x) + " " + ToStr(Vec2.y));
    }

    void FShader::SetUniform(const Str& Name, const glm::mat4 &Mat4) const {
        auto matrixData = (const float*)glm::value_ptr(Mat4);
        SetUniform4x4(Name, matrixData);

        CHECK_UNIFORM_ERRORS
    }

    void FShader::SetUniform(const Str &Name, const glm::vec3 &Vec3) const {
        this->Use();

        IN data = glm::value_ptr(Vec3);
        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform3fv(loc, 1, data);

        CHECK_UNIFORM_ERRORS
    }

    void FShader::SetUniform(const Str &Name, const glm::vec4 &Vec4) const {
        this->Use();

        IN data = glm::value_ptr(Vec4);
        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform4fv(loc, 1, data);

        CHECK_UNIFORM_ERRORS
    }

    void FShader::SetUniform(const Str &Name, const glm::mat3 &Mat3) const {
        auto matrixData = (const float*)glm::value_ptr(Mat3);

        SetUniform3x3(Name, matrixData);

        CHECK_UNIFORM_ERRORS
    }

    void FShader::SetUniform(const Str &Name, Texture& texture) {
        this->Use();

        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform1i(loc, texture.getTextureUnit());

        TextureUnits[texture.getTextureUnit()] = Slab::Naked(texture);

        CHECK_UNIFORM_ERRORS
    }

    void FShader::SetUniform(const Str &Name, const std::array<float, 3> Vec3) const {
        this->Use();

        fix loc = glGetUniformLocation(Handle, Name.c_str());
        CHECK_UNIFORM_EXISTS

        glUniform3fv( loc, 1, Vec3.data());

        CHECK_UNIFORM_ERRORS
    }

    void FShader::SetUniform(const Str &Name, const std::array<float, 4> Vec4) const {
        this->Use();

        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS
        glUniform4fv( loc, 1, Vec4.data());

        CHECK_UNIFORM_ERRORS
    }

    void FShader::SetUniform3x3(const Str &Name, const float *Mat3) const {
        this->Use();

        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS
        glUniformMatrix3fv( loc, 1, GL_FALSE, Mat3);

        CHECK_UNIFORM_ERRORS
    }

    void FShader::SetUniform4x4(const Str &Name, const float *Mat4) const {
        this->Use();

        fix loc = glGetUniformLocation(Handle, Name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniformMatrix4fv( loc, 1, GL_FALSE, Mat4);

        CHECK_UNIFORM_ERRORS
    }



} // OpenGL