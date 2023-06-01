//
// Created by joao on 29/05/23.
//

#include "Graph.h"


Base::Graphics::Graph2D::Graph2D(double xMin, double xMax, double yMin, double yMax, String title,
                                 bool filled, int samples)
                                 : xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), title(title),
                                   filled(filled), samples(samples) {       }

void Base::Graphics::Graph2D::_drawAxes(const Window *win) {

    glLineWidth(1.0);

    __computeSpacings();

    __drawXAxis(win);
    __drawYAxis(win);

    if(!labels.empty()){
        const double Sx = (xMax-xMin) / win->w;
        const double Sy = (yMax-yMin) / win->h;
        const double Tx = xMin;
        const double Ty = yMin;

        for(size_t i=0; i<labels.size(); ++i){
            labels[i]->draw(Sx, Sy, Tx, Ty);
        }
    }
}

void Base::Graphics::Graph2D::__computeSpacings() {
    double deltaY;
    double markStart = yMax;

    deltaY = yMax-yMin;


    while(markStart > yMin)
        markStart-=yspacing;
    while(markStart < (yMin-yspacing))
        markStart+=yspacing;

    yspacing = deltaY/10;
}

void Base::Graphics::Graph2D::__drawXAxis(const Window *win) {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    const double inPixelsTimes2 = 5;
    const double vTick = inPixelsTimes2 * (yMax-yMin) / win->h;
    const double hTick = inPixelsTimes2 * (xMax-xMin) / win->w;
    (void)hTick;

    auto &gtfColor = Styles::GetColorScheme()->graphTicksFont;

    double xspacing = (xMax-xMin) / 10.0;
    if(xspacing == .0) xspacing = 1.0;

    {
        glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);

        const double yloc = -yspacing*0.356;
        for (double mark = 0; mark <= xMax * 1.0001; mark += xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            GLUTUtils::writeOrtho(win, {xMin, xMax, yMin, yMax}, 1, mark - xspacing / 18.0, yloc, buffer, TICK_FONT);
        }
        for (double mark = 0; mark >= xMin * 1.0001; mark -= xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            GLUTUtils::writeOrtho(win, {xMin, xMax, yMin, yMax}, 1, mark - xspacing / 18.0, yloc, buffer, TICK_FONT);
        }
    }

    if(1)
    {
        auto &ac = Styles::GetColorScheme()->axisColor;
        auto &tc = Styles::GetColorScheme()->majorTickColor;
        glBegin(GL_LINES);
        {
            //glPopAttrib();

            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(xMin, 0, 0);
            glVertex3d(xMax, 0, 0);

            glColor4f(tc.r, tc.g, tc.b, tc.a);

            for(double mark = 0; mark<=xMax; mark+=xspacing){
                glVertex3d(mark, -vTick, 0);
                glVertex3d(mark, +vTick, 0);
            }
            for(double mark = 0; mark>=xMin; mark-=xspacing){
                glVertex3d(mark, -vTick, 0);
                glVertex3d(mark, +vTick, 0);
            }
        }
        glEnd();
    }


}

void Base::Graphics::Graph2D::__drawYAxis(const Window *win) {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    const double deltaY = yMax-yMin;
    const double deltaX = xMax-xMin;
    const double xloc = xMin-deltaX*0.05;

    // double magnitude = std::log10(std::abs(deltaY));

    auto markStart = yMin;

    StringStream buffer;
    //buffer << std::scientific << std::setprecision(2);

    auto &gtf = Styles::GetColorScheme()->graphTicksFont;
    glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
    {
        for(double mark = markStart; mark<=yMax; mark+=yspacing)
        {
            buffer.str("");
            if(fabs(mark) > deltaY*1.e-15)
                buffer << mark;
            else
                buffer << "0";
            GLUTUtils::writeOrtho(win, {xMin,xMax,yMin,yMax}, 1, float(xloc), float(mark),
                                  buffer.str().c_str(), TICK_FONT);
        }
    }


    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LINE_SMOOTH);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x2727);
    glLineStipple(2, 0x1249);
    glLineStipple(2, 0x1111);

    glBegin(GL_LINES);
    {
        auto &ac = Styles::GetColorScheme()->axisColor;
        auto &tc = Styles::GetColorScheme()->majorTickColor;

        glColor4f(tc.r, tc.g, tc.b, tc.a);

        for(double mark = markStart; mark<=yMax; mark+=yspacing ){
            glVertex3d(xMin, mark, 0);
            glVertex3d(xMax, mark, 0);
        }

        glColor4f(ac.r, ac.g, ac.b, ac.a);

        glVertex3d(xMin, 0, 0);
        glVertex3d(xMax, 0, 0);
    }
    glEnd();
    glPopAttrib();

}

void Base::Graphics::Graph2D::draw(const Window *window) {
    setupOrtho();

    //glMatrixMode(GL_MODELVIEW);

    auto &tf = Styles::GetColorScheme()->graphTitleFont;
    glColor4f(tf.r, tf.g, tf.b, tf.a);
    //writeOrtho(window, {xMin, xMax, yMin, yMax}, 2, -0.95, 0.85, title, FONT_STROKE_ROMAN);

    _drawAxes(window);

    _drawCurves(window);

}

void Base::Graphics::Graph2D::setupOrtho() {
    const double deltaX = xMax-xMin;
    const double deltaY = yMax-yMin;
    const double xTraLeft  = -deltaX*0.07;
    const double xTraRight = +deltaX*0.02;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xMin+xTraLeft, xMax+xTraRight, (yMin-deltaY*0.025), (yMax+deltaY*0.025), -1, 1);
}

void Base::Graphics::Graph2D::_nameLabelDraw(int i, const Styles::PlotStyle &style, String label,
                                             const Window *window) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1, 0, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    auto dx = .080,
            dy = -.060;
    auto xGap = 0.015,
            yGap = -.025;
    auto xMin = .100,
            xMax = xMin+dx,
            yMin = .975+(yGap+dy)*float(i),
            yMax = yMin+dy;

    if(style.filled) {
        auto color = style.fillColor;
        glColor4f(color.r, color.g, color.b, .5 * color.a);

        glRectd(xMin, yMin, xMax, yMax);
    }

    {
        auto color = style.lineColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glLineWidth(style.lineWidth);

        if (style.trace != Styles::Solid) {
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else {
            glEnable(GL_LINE_SMOOTH);
            glDisable(GL_LINE_STIPPLE);
        }

        glBegin(GL_LINE_LOOP);

        glVertex2f(xMin, yMin);
        glVertex2f(xMax, yMin);
        glVertex2f(xMax, yMax);
        glVertex2f(xMin, yMax);

        glEnd();

    }

    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.5);

    auto c = Styles::GetColorScheme()->graphTitleFont;
    glColor4f(c.r,c.g,c.b,c.a);
    GLUTUtils::writeOrtho(window, Rect{0,1,0,1}, 1, xMax+xGap, .5*(yMax+yMin), label);

    glPopMatrix();
//
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Base::Graphics::Graph2D::addLabel(Label *label) { labels.push_back(label); }

Real Base::Graphics::Graph2D::get_yMin() const {
    return yMin;
}

Real Base::Graphics::Graph2D::get_yMax() const {
    return yMax;
}

void
Base::Graphics::Graph2D::addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, String name) {
    CurveTriple triple = {curve, style, name};
    curves.emplace_back(triple);
}

void Base::Graphics::Graph2D::_drawCurves(const Window *win) {
    for(auto curveTriple : curves) {
        auto curve = GetCurve(curveTriple);
        auto pointSet = curve.get()->getAsPointSet();
        auto points = pointSet.get()->getPoints();

        if(points.size()<2) continue;

        auto style = GetStyle(curveTriple);
        auto name  = GetName(curveTriple);

        auto color = style.lineColor;

        glColor4f(color.r, color.g, color.b, color.a);
        glLineWidth(style.lineWidth);

        if (style.trace != Styles::Solid) {
            glDisable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(style.stippleFactor, style.stipplePattern);
        } else {
            glEnable(GL_LINE_SMOOTH);
            glDisable(GL_LINE_STIPPLE);
        }

        glBegin(GL_LINE_STRIP);
        {
            for(const auto &p : points)
                glVertex2d(p.x, p.y);
        }
        glEnd();
    }
}

void Base::Graphics::Graph2D::clearCurves() {
    curves.clear();
}



