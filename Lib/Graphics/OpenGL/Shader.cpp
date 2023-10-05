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


namespace Graphics::OpenGL {
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
            GLchar name[maxLength]; // Variable to hold the uniform name

            glGetActiveUniform(handle, (GLuint)i, maxLength, &length, &size, &type, name);

            GLint location = glGetUniformLocation(handle, name);

            log << "\n\t\t\t\t\t"
                << (i+1) << "/" << numActiveUniforms << " @ location " << location << " : \""
                << name << "\" (" << GLTypeToGLSLType(type) << ")";
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
            GLchar name[maxLength]; // Variable to hold the uniform name

            // Get information about the i-th active attribute
            glGetActiveAttrib(program, i, maxLength, &length, &size, &type, name);

            // Get the location of the attribute
            GLint location = glGetAttribLocation(program, name);

            log << "\n\t\t\t\t\t"
                << (i+1) << "/" << numAttributes << " @ location " << location << " : \""
                << name << "\" (" << GLTypeToGLSLType(type) << ")" ;
        }
    }




    Shader::Shader(const Str& vertFilename, const Str& fragFilename) {
        Core::BackendManager::LoadModule(Core::ModernOpenGL);

        handle = ShaderLoader::Load(vertFilename, fragFilename);

        Log::Info() << "Shader files '" << vertFilename
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
        if(!Core::BackendManager::IsModuleLoaded(Core::ModernOpenGL)) return;
        glUseProgram(0);
    }

    void Shader::setUniform(const Str& name, GLint value) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());
        assert(loc != -1);

        glUniform1i(loc, value);

        checkGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + name + Log::FGBlue + "' = " + ToStr(value));
    }

    void Shader::setUniform(const Str& name, GLfloat value) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());
        assert(loc != -1);

        glUniform1f(loc, value);

        checkGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + name + Log::FGBlue + "' = " + ToStr(value));
    }

    void Shader::setUniform(const Str &name, Real2D vec2) const {
        this->use();

        fix loc = glGetUniformLocation(handle, name.c_str());
        assert(loc != -1);

        glUniform2f(loc, (GLfloat)vec2.x, (GLfloat)vec2.y);

        checkGLErrors(Str(__PRETTY_FUNCTION__) + " : '" + Log::FGGreen + name + Log::FGBlue + "' = " + ToStr(vec2.x) + " " + ToStr(vec2.y));
    }

    void Shader::setUniform(const Str& name, const glm::mat4 &mat4) const {
        auto matrixData = (const float*)glm::value_ptr(mat4);
        setUniform4x4(name, matrixData);

        checkGLErrors(__PRETTY_FUNCTION__);
    }

    void Shader::setUniform(const Str &name, const glm::mat3 &mat3) const {
        auto matrixData = (const float*)glm::value_ptr(mat3);

        setUniform3x3(name, matrixData);

        checkGLErrors(__PRETTY_FUNCTION__);
    }

    void Shader::setUniform(const Str &name, Texture& texture) {
        this->use();

        glUniform1i(glGetUniformLocation(handle, name.c_str()), texture.getTextureUnit());

        textureUnits[texture.getTextureUnit()] = DummyPtr(texture);

        checkGLErrors(__PRETTY_FUNCTION__);
    }

    void Shader::setUniform4x4(const Str &name, const float *mat4) const {
        this->use();
        glUniformMatrix4fv( glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, mat4);

        checkGLErrors(__PRETTY_FUNCTION__);
    }

    void Shader::setUniform3x3(const Str &name, const float *mat3) const {
        this->use();
        glUniformMatrix3fv( glGetUniformLocation(handle, name.c_str()), 1, GL_FALSE, mat3);

        checkGLErrors(__PRETTY_FUNCTION__);
    }



} // OpenGL