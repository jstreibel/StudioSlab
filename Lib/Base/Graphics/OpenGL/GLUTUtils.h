//
// Created by joao on 10/29/21.
//

#ifndef STUDIOSLAB_GLUTDEFS_H
#define STUDIOSLAB_GLUTDEFS_H

#include "Base/Graphics/Artists/Rect.h"

#include "Common/STDLibInclude.h"

#include "Utils.h"

#include <GL/freeglut.h>

#define FONT_BITMAP_1 GLUT_STROKE_ROMAN
#define FONT_BITMAP_2 GLUT_STROKE_MONO_ROMAN
#define FONT_BITMAP_3 GLUT_BITMAP_9_BY_15
#define FONT_BITMAP_4 GLUT_BITMAP_8_BY_13
#define FONT_BITMAP_5 GLUT_BITMAP_TIMES_ROMAN_10
#define FONT_BITMAP_6 GLUT_BITMAP_TIMES_ROMAN_24
#define FONT_BITMAP_7 GLUT_BITMAP_HELVETICA_10
#define FONT_BITMAP_8 GLUT_BITMAP_HELVETICA_12
#define FONT_BITMAP_9 GLUT_BITMAP_HELVETICA_18

#define FONT_STROKE_ROMAN GLUT_STROKE_ROMAN
#define FONT_STROKE_ROMAN_MONO GLUT_STROKE_ROMAN_MONO


#define FONT_BITMAP_DEFAULT FONT_BITMAP_6
#define FONT_STROKE_DEFAULT FONT_STROKE_ROMAN


class Window;

namespace GLUTUtils {

    void write(const Window *window, float fontScale, float x, float y, std::string str,
               void *font = FONT_STROKE_DEFAULT);

    void writeOrtho(const Window *window, Rect region, float fontScale, float x, float y, std::string str,
                    void *font = FONT_STROKE_DEFAULT);

    void writeBitmap(const Window *window, float x, float y, std::string str,
                     void *font = FONT_BITMAP_DEFAULT);

    OpenGLUtils::FrameBuffer getFrameBuffer(int x=0, int y=0, int w=-1, int h=-1);

    int getScreenWidth();

    int getScreenHeight();

}

#endif //STUDIOSLAB_GLUTDEFS_H
