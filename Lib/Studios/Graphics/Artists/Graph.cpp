//
// Created by joao on 3/31/23.
//

#include "Graph.h"

void auxRectDraw(int i, Color color, String label, const Window *window) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    auto dx = .120,
         dy = -.060;
    auto xGap = 0.015,
         yGap = -.025;
    auto xMin = .100,
         xMax = xMin+dx,
         yMin = .975+(yGap+dy)*float(i),
         yMax = yMin+dy;

    glColor4f(color.r, color.g, color.b, .5*color.a);

    glRectd(xMin,yMin,xMax,yMax);

    glColor4f(color.r, color.g, color.b, color.a);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);

    glVertex2f(xMin, yMin);
    glVertex2f(xMax, yMin);
    glVertex2f(xMax, yMax);
    glVertex2f(xMin, yMax);

    glEnd();

    auto c = ColorScheme::graphTitleFont;
    glColor4f(c.r,c.g,c.b,c.a);
    writeOrtho(window, Rect{0,1,0,1}, 1, xMax+xGap, .5*(yMax+yMin), label);

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}
