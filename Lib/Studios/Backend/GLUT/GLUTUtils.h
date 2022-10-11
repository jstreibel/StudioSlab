//
// Created by joao on 10/29/21.
//

#ifndef STUDIOSLAB_GLUTDEFS_H
#define STUDIOSLAB_GLUTDEFS_H

#include <Common/STDLibInclude.h>

#include <GL/glut.h>

#define FONT1 GLUT_STROKE_ROMAN
#define FONT2 GLUT_STROKE_MONO_ROMAN
#define FONT3 GLUT_BITMAP_9_BY_15
#define FONT4 GLUT_BITMAP_8_BY_13
#define FONT5 GLUT_BITMAP_TIMES_ROMAN_10
#define FONT6 GLUT_BITMAP_TIMES_ROMAN_24
#define FONT7 GLUT_BITMAP_HELVETICA_10
#define FONT8 GLUT_BITMAP_HELVETICA_12
#define FONT9 GLUT_BITMAP_HELVETICA_18

#define DEFAULT_FONT FONT6

void write(float x, float y, std::string str,
           void *font = DEFAULT_FONT);

#endif //STUDIOSLAB_GLUTDEFS_H
