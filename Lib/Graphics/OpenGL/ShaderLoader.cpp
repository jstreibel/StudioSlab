//
// Created by joao on 4/10/23.
//

#include "ShaderLoader.h"

#include "Utils/Resources.h"

#include "rougier/shader.h"
#include "Core/Tools/Log.h"

#include "Graphics/OpenGL/rougier/shader.h"

#include <fstream>

namespace Graphics::OpenGL {

    fix shadersRoot = Resources::ShadersFolder;

    Str _parseGLSL(const Str& filePath, std::unordered_set<std::string> &includedFiles) {
        // Check if the file has already been included
        if (includedFiles.find(filePath) != includedFiles.end())
            return "";

        includedFiles.insert(filePath);

        for(auto &incFile : includedFiles)
            Log::Debug() << incFile << Log::Flush;


        std::ifstream file(filePath);
        if (!file.is_open()) {
            Log::Error() << "Failed to open shader file: " << filePath << Log::Flush;
            throw Exception("while trying to compile glsl shader");
        }


        StringStream ss;
        Str line;
        while (std::getline(file, line)) {
            line.erase(line.begin(), std::find_if(line.begin(), line.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));

            // Skip lines that start with "//"
            if (line.substr(0, 2) == "//") continue;

            if (line.find("#include") != std::string::npos) {
                // Extract the included file name
                size_t start = line.find('\"') + 1;
                size_t end = line.find_last_of('\"');
                Str includeFile = shadersRoot + line.substr(start, end - start);

                Log::Note() << "Including file " << includeFile << Log::Flush;

                // Parse the included file recursively
                Str includedContent = _parseGLSL(includeFile, includedFiles);

                // Append the included content to the output
                ss << includedContent << "\n";

            } else
                ss << line << "\n";
        }

        return ss.str();

    }

    Str ParseGLSLSource(const Str& filePath) {
        std::unordered_set<std::string> included;
        return _parseGLSL(filePath, included);
    }

    GLuint ShaderLoader::Compile(const Str &source, ShaderType type) {
        GLint compile_status;
        GLuint handle = glCreateShader( type );

        const char *rawSource = source.c_str();

        glShaderSource( handle, 1, &rawSource, 0 );
        glCompileShader( handle );

        glGetShaderiv( handle, GL_COMPILE_STATUS, &compile_status );
        if( compile_status == GL_FALSE )
        {
            GLchar messages[256];
            glGetShaderInfoLog( handle, sizeof(messages), 0, &messages[0] );
            Log::Error() << messages;
            throw Exception("while trying to compile glsl shader");
        }
        return handle;    }

    GLuint ShaderLoader::Load(const Str &vertFilename, const Str &fragFilename) {
        GLuint handle = glCreateProgram( );
        GLint link_status;

        if( !vertFilename.empty() )
        {
            auto vertSource = ParseGLSLSource( vertFilename );
            GLuint vertShader = Compile(vertSource, VertexShader);
            glAttachShader( handle, vertShader);
            glDeleteShader( vertShader );
        }
        if( !fragFilename.empty() )
        {
            auto fragSource = ParseGLSLSource( fragFilename );
            GLuint fragShader = Compile( fragSource, FragmentShader);
            glAttachShader( handle, fragShader);
            glDeleteShader( fragShader );
        }

        glLinkProgram( handle );

        glGetProgramiv( handle, GL_LINK_STATUS, &link_status );
        if (link_status == GL_FALSE)
        {
            GLchar messages[256];
            glGetProgramInfoLog( handle, sizeof(messages), nullptr, &messages[0] );
            Log::Error() << messages;
            throw Exception("while trying to compile glsl shader");
        }
        return handle;
    }
} // Graphics::OpenGL