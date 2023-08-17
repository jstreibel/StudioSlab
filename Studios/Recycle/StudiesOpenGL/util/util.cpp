//
// Created by joao on 08/04/2021.
//

#include <fstream>
#include <sstream>
#include "util.h"

/*
void checkOpenGLDebugExtension ()
{
    int NumberOfExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    for(int i=0; i<NumberOfExtensions; i++) {
        const GLubyte *ccc=glGetStringi(GL_EXTENSIONS, i);

        if ( strcmp(ccc, (const GLubyte *)"GL_ARB_debug_output") == 0 ) {
            // The extension is supported by our hardware and driver
            // Try to get the "glDebugMessageCallbackARB" function :
            glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC) wglGetProcAddress(
                    "glDebugMessageCallbackARB");
        }
    }

    throw "checkOpenGLDebugExtension not implemented.";
}
*/

bool fileExists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

std::string loadFileToString(std::string fileName) {
    if(!fileExists(fileName)) return std::string("");

    std::ifstream t(fileName);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}
