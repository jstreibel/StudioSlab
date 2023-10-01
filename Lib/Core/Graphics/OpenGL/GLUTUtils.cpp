//
// Created by joao on 10/29/21.
//

#include "GLUTUtils.h"

#include "Core/Graphics/Window/Window.h"

void Graphics::OpenGL::GLUT::write(const Window *win, const float fontScale, const float x, const float y, std::string str, void *font)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(x,y,0);
    glScalef(fontScale, fontScale, 1);
    glutStrokeString(font, (unsigned char*)str.c_str());

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Graphics::OpenGL::GLUT::writeOrtho(const Window *window, RectR region, float fontScale, float x, float y, std::string str,
                void *font) {
    fix w = region.xMax-region.xMin;
    fix h = region.yMax-region.yMin;
    fix regionRatio = w/(Real)h;
    fix windowRatio = window->getw()/(Real)window->geth();

    fix baseScale = h*2.0e-4;

    fix importantRatio = regionRatio/windowRatio;

    fix xScale = fontScale*baseScale*importantRatio,
        yScale = fontScale*baseScale;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x, y, 0);
    glScalef(xScale, yScale, 1);
    for(int i=0;i<1;++i)
        glutStrokeString(font, (unsigned char *) str.c_str());

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Graphics::OpenGL::GLUT::writeBitmap(const Window *window, float x, float y, std::string str, void *font){
    glRasterPos2f(x, y);
    for(char c : str) glutBitmapCharacter(font, c);
}

Graphics::OpenGL::FrameBuffer Graphics::OpenGL::GLUT::getFrameBuffer(int x, int y, int w_, int h_) {
    const auto w = w_<0 ? glutGet(GLUT_WINDOW_WIDTH)  : w_;
    const auto h = h_<0 ? glutGet(GLUT_WINDOW_HEIGHT) : h_;
    const auto channels = Graphics::OpenGL::FrameBuffer::channels;

    std::vector<Graphics::OpenGL::FrameBuffer::DataType> pixels(w*h*channels);

    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);

    return {w, h, pixels};
}

int Graphics::OpenGL::GLUT::getScreenWidth() {
    return glutGet(GLUT_WINDOW_WIDTH);
}

int Graphics::OpenGL::GLUT::getScreenHeight() {
    return glutGet(GLUT_WINDOW_HEIGHT);
}
