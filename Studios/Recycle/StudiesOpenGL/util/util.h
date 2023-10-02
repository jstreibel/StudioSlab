//
// Created by joao on 08/04/2021.
//

#ifndef OPENGLTUTORIAL_UTIL_H
#define OPENGLTUTORIAL_UTIL_H

//#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#include "Graphics/OpenGL/OpenGL.h"
#include <string>

void checkOpenGLDebugExtension ();

bool fileExists (const std::string& name);

std::string loadFileToString(std::string fileName);



#endif //OPENGLTUTORIAL_UTIL_H
