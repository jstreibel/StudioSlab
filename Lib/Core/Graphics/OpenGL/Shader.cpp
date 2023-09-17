//
// Created by joao on 16/09/23.
//

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "rougier/shader.h"
#include "Core/Tools/Log.h"


namespace OpenGL {
    Shader::Shader(const Str& vertFilename, const Str& fragFilename) {
        handle = ftgl::shader_load(vertFilename.c_str(), fragFilename.c_str());

        GLint numActiveUniforms = 0;
        glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

        auto &log = Log::Info() << "Shader files '" << vertFilename
                                       << "' and '" << fragFilename << "' loaded and compiled. Active uniforms:";

        for (GLint i = 0; i < numActiveUniforms; ++i) {
            GLsizei length;
            GLint size;
            GLenum type;
            const GLsizei bufSize = 64; // Maximum name length
            GLchar name[bufSize]; // Variable to hold the uniform name

            glGetActiveUniform(handle, (GLuint)i, bufSize, &length, &size, &type, name);

            GLint location = glGetUniformLocation(handle, name);

            log << "\n\t\t\t\t\t" << "Uniform #" << i << " Type: " << type << " Name: " << name << " Location: " << location;
        }

        log << Log::Flush;

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