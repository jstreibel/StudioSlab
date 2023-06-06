//
// Created by joao on 3/31/23.
//

#include <GL/glew.h>

#include "GLUTUtils.h"
#include "Utils.h"
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

bool OpenGLUtils::outputToPNG(Window *window, std::string fileName, int width, int height) {
    // Create texture:
    GLuint texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Create framebuffer object
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);


    // Check..
    auto retVal = true;
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR outputting png: Framebuffer is not complete!" << std::endl;

        retVal = false;
    } else {
        auto xOld = window->getx();
        auto yOld = window->gety();
        auto wOld = window->getw();
        auto hOld = window->geth();

        window->setx(0);
        window->sety(0);
        window->setSize(width, height);

        glClear(GL_COLOR_BUFFER_BIT);

        window->draw();

        window->setx(xOld);
        window->sety(yOld);
        window->setSize(wOld, hOld);

        glFlush();

        auto buffer = GLUTUtils::getFrameBuffer(0,0,width,height);

        outputToPNG(buffer, fileName);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &texColorBuffer);

    return retVal;

}
void OpenGLUtils::piccolos() { }

