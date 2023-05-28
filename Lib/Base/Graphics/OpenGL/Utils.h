//
// Created by joao on 3/31/23.
//

#ifndef STUDIOSLAB_OPENGLUTILS_H
#define STUDIOSLAB_OPENGLUTILS_H

#include "../Artists/Rect.h"

namespace OpenGLUtils {
    void drawOrthoNormalized(Rect rect);

    bool outputToPNG(std::vector<uint8_t> pixelData, int width, int height, int bitspp, std::string fileName);

    void piccolos();
}

#endif //STUDIOSLAB_OPENGLUTILS_H
