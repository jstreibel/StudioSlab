//
// Created by joao on 4/10/23.
//

#include "ShaderLoader.h"

#include "rougier/shader.h"
#include "Core/Tools/Log.h"
#include "Graphics/OpenGL/rougier/shader.h"

#include <fstream>

namespace Graphics::OpenGL {
    Str ParseGLSL(const Str& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            Log::Error() << "Failed to open shader file: " << filePath << Log::Flush;
            return "";
        }

        std::stringstream ss;
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("#include") != std::string::npos) {
                // Extract the included file name
                size_t start = line.find('\"') + 1;
                size_t end = line.find_last_of('\"');
                std::string includeFile = line.substr(start, end - start);

                // Parse the included file recursively
                std::string includedContent = ParseGLSL(includeFile);

                // Append the included content to the output
                ss << includedContent << "\n";
            } else {
                ss << line << "\n";
            }
        }

        return ss.str();
    }


    GLuint ShaderLoader::Load(const Str &vertFilename, const Str &fragFilename) {
        GLuint handle = glCreateProgram( );
        GLint link_status;

        if( !vertFilename.empty() )
        {
            char *vert_source = ftgl::shader_read( vertFilename.c_str() );

            GLuint vert_shader = ftgl::shader_compile( vert_source, GL_VERTEX_SHADER);
            glAttachShader( handle, vert_shader);
            glDeleteShader( vert_shader );
            free( vert_source );
        }
        if( !fragFilename.empty() )
        {
            char *frag_source = ftgl::shader_read( fragFilename.c_str() );
            GLuint frag_shader = ftgl::shader_compile( frag_source, GL_FRAGMENT_SHADER);
            glAttachShader( handle, frag_shader);
            glDeleteShader( frag_shader );
            free( frag_source );
        }

        glLinkProgram( handle );

        glGetProgramiv( handle, GL_LINK_STATUS, &link_status );
        if (link_status == GL_FALSE)
        {
            GLchar messages[256];
            glGetProgramInfoLog( handle, sizeof(messages), 0, &messages[0] );
            fprintf( stderr, "%s\n", messages );
            exit(1);
        }
        return handle;
    }
} // Graphics::OpenGL