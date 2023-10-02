//
// Created by joao on 08/04/2021.
//

#ifndef OPENGLTUTORIAL_PROGRAM_H
#define OPENGLTUTORIAL_PROGRAM_H

#include "Graphics/OpenGL/OpenGL.h"
#include <string>

class Program {


public:
    Program();

    void loadShaders(std::string vertFile, std::string fragFile);
    void bind();

    void setFloat(std::string name, GLfloat value);
    void setInt(std::string name, GLint value);

private:

private:
    bool loaded = false;
    GLuint mProgramID;

};


#endif //OPENGLTUTORIAL_PROGRAM_H
