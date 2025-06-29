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

#define CHECK_UNIFORM_EXISTS \
    if(loc == -1){               \
    Log::Error("While setting uniform '") << name << "'. Uniform not found, " \
        << "might have been pruned by shader compiler."; \
    return; \
    }

#define CHECK_UNIFORM_ERRORS \
    CheckGLErrors(Str("uniform '") + name + "' @ " + __PRETTY_FUNCTION__);

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

            glGetActiveUniform(handle, (GLuint)i, maxLength, &length, &size, &type, &Name[0]);

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




    Shader::Shader(const Str& vertFilename, const Str& fragFilename)
    : vertFileName(vertFilename)
    , fragFileName(fragFilename)
    {
        Core::LoadModule("ModernOpenGL");

        handle = ShaderLoader::Load(vertFilename, fragFilename);

        Log::Note() << "Shader files '" << vertFilename
                                << "' and '" << fragFilename << "' loaded and compiled." << Log::Flush;
        auto &log = Log::Debug() << "Are active in this shader:";

        log << "\n\t\t\t\tUniforms:";
        ListShaderUniforms(handle, log);

        log << "\n\t\t\t\tVertex attributes:";
        ListShaderAttributes(handle, log);
        log << Log::Flush;
    }

    void Shader::bindTextures() const {
        for(auto &texUnit : textureUnits)
            texUnit.second->bind();
    }

    void Shader::use() const {
        bindTextures();

        glUseProgram(handle);
    }

    void Shader::remove() {
        if(!Core::BackendManager::IsModuleLoaded("ModernOpenGL")) return;
        glUseProgram(0);
    }

    void Shader::setUniform(const Str& name, GLint value) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform1i(loc, value);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + name + Log::FGBlue + "' = " + ToStr(value));
    }

    void Shader::setUniform(const Str& name, GLfloat value) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform1f(loc, value);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + name + Log::FGBlue + "' = " + ToStr(value));
    }

    void Shader::setUniform(const Str &name, Real2D vec2) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform2f(loc, (GLfloat)vec2.x, (GLfloat)vec2.y);

        CheckGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + name + Log::FGBlue + "' = " + ToStr(vec2.x) + " " + ToStr(vec2.y));
    }

    void Shader::setUniform(const Str& name, const glm::mat4 &mat4) const {
        auto matrixData = (const float*)glm::value_ptr(mat4);
        setUniform4x4(name, matrixData);

        CHECK_UNIFORM_ERRORS
    }

    void Shader::setUniform(const Str &name, const glm::vec3 &vec3) const {
        this->use();

        IN data = glm::value_ptr(vec3);
        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform3fv(loc, 1, data);

        CHECK_UNIFORM_ERRORS
    }

    void Shader::setUniform(const Str &name, const glm::vec4 &vec4) const {
        this->use();

        IN data = glm::value_ptr(vec4);
        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform4fv(loc, 1, data);

        CHECK_UNIFORM_ERRORS
    }

    void Shader::setUniform(const Str &name, const glm::mat3 &mat3) const {
        auto matrixData = (const float*)glm::value_ptr(mat3);

        setUniform3x3(name, matrixData);

        CHECK_UNIFORM_ERRORS
    }

    void Shader::setUniform(const Str &name, Texture& texture) {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniform1i(loc, texture.getTextureUnit());

        textureUnits[texture.getTextureUnit()] = Slab::Naked(texture);

        CHECK_UNIFORM_ERRORS
    }

    void Shader::setUniform(const Str &name, std::array<float, 3> vec3) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());
        CHECK_UNIFORM_EXISTS

        glUniform3fv( loc, 1, vec3.data());

        CHECK_UNIFORM_ERRORS
    }

    void Shader::setUniform(const Str &name, std::array<float, 4> vec4) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS
        glUniform4fv( loc, 1, vec4.data());

        CHECK_UNIFORM_ERRORS
    }

    void Shader::setUniform3x3(const Str &name, const float *mat3) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS
        glUniformMatrix3fv( loc, 1, GL_FALSE, mat3);

        CHECK_UNIFORM_ERRORS
    }

    void Shader::setUniform4x4(const Str &name, const float *mat4) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());

        CHECK_UNIFORM_EXISTS

        glUniformMatrix4fv( loc, 1, GL_FALSE, mat4);

        CHECK_UNIFORM_ERRORS
    }



} // OpenGL