//
// Created by joao on 8/8/22.
//

#include "StatsDisplay.h"

#include "Studios/Graphics/WindowManagement/Window.h"
#include "Studios/Backend/GLUT/GLUTUtils.h"

#include <GL/gl.h>

void StatsDisplay::addVolatileStat(const String &stat, const Color color)
{
    stats.push_back({stat, color});
}

void StatsDisplay::draw(const Window *window) {
    auto w = float(window->w), h = float(window->h);

    float scaleX = 2.f/w, scaleY = 2.f*2.f/h;
    glScalef(scaleX, scaleY, 1);
    glTranslatef(-w/2.f, h/4.f, 0);


    {
        const int cursorx = 14;
        const float lineHeight = 18;
        float delta = lineHeight;
        auto font = FONT9;

        auto colorTone = char(0.9*255);
        glColor3b(colorTone, colorTone, colorTone);

        for(auto stat : stats){
            auto color = stat.second;
            glColor3f(color.r, color.g, color.b);
            write(cursorx, -delta, stat.first, font);
            delta += lineHeight;
        }
        stats.clear();
    }
}
