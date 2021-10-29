//
// Created by joao on 10/29/21.
//

#include "GLUTUtils.h"

void write(float x, float y, std::string str, void *font)
{
    glRasterPos2f(x, y);
    for(char c : str) glutBitmapCharacter(font, c);
}