//
// Created by joao on 3/31/23.
//

#include "Utils.h"

#include <GL/gl.h>
#include <FreeImagePlus.h>

#include <cstring>
#include <iostream>

void OpenGLUtils::drawOrthoNormalized(Rect rect) {
    // TODO ultra-provisório
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glRectd(rect.xMin, rect.yMin, rect.xMax, rect.yMax);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

bool OpenGLUtils::outputToPNG(std::vector<uint8_t> pixelData, int width, int height, int bitspp,
                              std::string fileName)
{
    FIBITMAP *image = FreeImage_Allocate(width, height, bitspp);
    if (!image) {
        std::cerr << "Failed to allocate image!" << std::endl;
        return false;
    }


    BYTE* bits = FreeImage_GetBits(image);
    memcpy(bits, &pixelData[0], width * height * bitspp / 8);


    bool success = FreeImage_Save(FIF_PNG, image, fileName.c_str(), PNG_DEFAULT);
    if (!success)
        std::cerr << "Failed to save the image." << std::endl;
    else std::cout << "Image \"" << fileName << "\" saved successfully!" << std::endl;

    FreeImage_Unload(image);

    return success;
}

void OpenGLUtils::piccolos() { }