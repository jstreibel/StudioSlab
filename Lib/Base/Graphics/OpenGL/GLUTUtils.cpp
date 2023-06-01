//
// Created by joao on 10/29/21.
//

#include "GLUTUtils.h"

#include "Base/Graphics/Window/Window.h"

void GLUTUtils::write(const Window *win, const float fontScale, const float x, const float y, std::string str, void *font)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(x,y,0);
    glScalef(fontScale, fontScale, 1);
    glutStrokeString(font, (unsigned char*)str.c_str());
    glPopMatrix();
}

void GLUTUtils::writeOrtho(const Window *window, Rect region, float fontScale, float x, float y, std::string str,
                void *font) {
    const auto w = region.xMax-region.xMin;
    const auto h = region.yMax-region.yMin;
    auto regionRatio = w/(double)h;
    auto windowRatio = window->getw()/(double)window->geth();

    auto baseScale = h*2.5e-4;

    auto xScale = fontScale*baseScale*regionRatio/windowRatio,
         yScale = fontScale*baseScale;

    glMatrixMode(GL_MODELVIEW);

    for(int i=0;i<3;++i) {
        glPushMatrix();


        glTranslatef(x, y, 0);
        glScalef(xScale, yScale, 1);

        glutStrokeString(font, (unsigned char *) str.c_str());

        glPopMatrix();
    }
}

void GLUTUtils::writeBitmap(const Window *window, float x, float y, std::string str, void *font){
    glRasterPos2f(x, y);
    for(char c : str) glutBitmapCharacter(font, c);
}

OpenGLUtils::FrameBuffer GLUTUtils::getFrameBuffer() {
    const auto w = glutGet(GLUT_WINDOW_WIDTH);
    const auto h = glutGet(GLUT_WINDOW_HEIGHT);
    const auto channels = OpenGLUtils::FrameBuffer::channels;

    std::vector<OpenGLUtils::FrameBuffer::DataType> pixels(w*h*channels);

    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    return {w, h, pixels};
}

int GLUTUtils::getScreenWidth() {
    return glutGet(GLUT_WINDOW_WIDTH);
}

int GLUTUtils::getScreenHeight() {
    return glutGet(GLUT_WINDOW_HEIGHT);
}
