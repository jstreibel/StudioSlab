//
// Created by joao on 10/29/21.
//

#include "GLUTUtils.h"

#include "../../Graphics/WindowManagement/Window.h"

void write(const Window *win, const float fontScale, const float x, const float y, std::string str, void *font)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(x,y,0);
    glScalef(fontScale, fontScale, 1);
    glutStrokeString(font, (unsigned char*)str.c_str());
    glPopMatrix();
}

void writeOrtho(const Window *window, Rect region, float fontScale, float x, float y, std::string str,
                void *font) {
    const auto w = region.xMax-region.xMin;
    const auto h = region.yMax-region.yMin;
    auto regionRatio = w/(double)h;
    auto windowRatio = window->w/(double)window->h;

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

void writeBitmap(const Window *window, float x, float y, std::string str, void *font){
    glRasterPos2f(x, y);
    for(char c : str) glutBitmapCharacter(font, c);
}