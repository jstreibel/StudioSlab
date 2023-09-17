//
// Created by joao on 16/09/23.
//

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "rougier/shader.h"
#include "Core/Tools/Log.h"
#include "Utils.h"


namespace OpenGL {
    void ListShaderUniforms(GLuint handle, std::basic_ostream<char, std::char_traits<char>> &log) {
        GLint numActiveUniforms = 0;
        glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

        for (GLint i = 0; i < numActiveUniforms; ++i) {
            GLsizei length;
            GLint size;
            GLenum type;
            const GLsizei bufSize = 64; // Maximum name length
            GLchar name[bufSize]; // Variable to hold the uniform name

            glGetActiveUniform(handle, (GLuint)i, bufSize, &length, &size, &type, name);

            GLint location = glGetUniformLocation(handle, name);

            log << "\n\t\t\t\t\t"
                << "Uniform #"  << std::left << std::setw(8) << i
                << "Location: " << std::left << std::setw(12) << location
                << "Type: "     << std::left << std::setw(20) << OpenGLUtils::GLTypeToGLSLType(type)
                << "Name: "     << std::left << std::setw(20) << name
                    ;
        }

        log << Log::Flush;

    }

    void ListShaderAttributes(GLuint program, std::basic_ostream<char, std::char_traits<char>> &log) {
        GLint numAttributes;
        GLint maxLength;

        // Get the number of active attributes
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttributes);

        // Get the maximum length of attribute names
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLength);

        // Allocate space for the name of each attribute
        GLchar *attributeName = new GLchar[maxLength];

        std::cout << "Listing active attributes:" << std::endl;

        for (GLint i = 0; i < numAttributes; ++i) {
            GLint size;
            GLenum type;
            GLsizei length;

            // Get information about the i-th active attribute
            glGetActiveAttrib(program, i, maxLength, &length, &size, &type, attributeName);

            // Get the location of the attribute
            GLint location = glGetAttribLocation(program, attributeName);

            std::cout << "Attribute " << i
                      << ": Name = " << attributeName
                      << ", Location = " << location
                      << ", Type = " << type
                      << ", Size = " << size
                      << std::endl;
        }

        // Clean up
        delete[] attributeName;
    }




    Shader::Shader(const Str& vertFilename, const Str& fragFilename) {
        handle = ftgl::shader_load(vertFilename.c_str(), fragFilename.c_str());
        auto &log = Log::Info() << "Shader files '" << vertFilename
                                << "' and '" << fragFilename << "' loaded and compiled. Active uniforms:";

        ListShaderUniforms(handle, log);

        ListShaderAttributes(handle, log);

    }

    void Shader::use() const { glUseProgram(handle); }

    void Shader::remove() const { glUseProgram(0); }

    void Shader::setUniform(const Str& name, GLint value) const {
        this->use();
        glUniform1i( glGetUniformLocation(handle, name.c_str()), value);
    }

    void Shader::setUniform(const Str& name, const glm::mat4 &mat4) const {
        auto matrixData = (const float*)glm::value_ptr(mat4);
        setUniform4x4(name, matrixData);
    }

    void Shader::setUniform4x4(const Str &name, const float *mat4) const {
        this->use();
        glUniformMatrix4fv( glGetUniformLocation(handle, name.c_str()), 1, 0, mat4);
    }


} // OpenGL