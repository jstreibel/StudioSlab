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

bool OpenGLUtils::outputToPNG(OpenGLUtils::FrameBuffer buffer,
                              std::string fileName)
{
    const auto w = buffer.w, h = buffer.h;
    const auto bpp = buffer.getBitsPerPixel();
    const auto pixelData = buffer.getPixelData();
    const auto dataSize = w*h*bpp/8;

    FIBITMAP *image = FreeImage_Allocate(w, h, bpp);
    if (!image) {
        std::cerr << "Failed to allocate image!" << std::endl;
        return false;
    }


    BYTE* bits = FreeImage_GetBits(image);
    memcpy(bits, pixelData, dataSize);


    bool success = FreeImage_Save(FIF_PNG, image, fileName.c_str(), PNG_DEFAULT);
    if (!success)
        std::cerr << "Failed to save the image." << std::endl;
    else std::cout << "Image \"" << fileName << "\" saved successfully!" << std::endl;

    FreeImage_Unload(image);

    return success;
}

void OpenGLUtils::piccolos() { }