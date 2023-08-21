//
// Created by joao on 29/05/23.
//

#include "Graph.h"
#include "imgui.h"

#include "Common/Printing.h"
#include "Base/Tools/Log.h"

#include "Base/Backend/GLUT/GLUTBackend.h"
#include "Base/Controller/Interface/InterfaceManager.h"

#define MARK                                                                                    \
    {                                                                                           \
    buffer.str("");                                                                             \
    auto numRegion = log10(deltaY);                                                             \
    buffer << std::setprecision(numRegion>2?0:numRegion>1?1:2)                                  \
           << (numRegion< -1 ? std::scientific : std::fixed) << mark;                           \
    GLUTUtils::writeOrtho(this, {xMin,xMax,yMin,yMax}, fontScale, float(xMarkingsLabels), float(mark),  \
            buffer.str().c_str(), TICK_FONT);                                                   \
    }

Base::Graphics::Graph2D::Graph2D(Real xMin, Real xMax, Real yMin, Real yMax, Str title,
                                 bool filled, int samples)
                                 : xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), title(title),
                                   filled(filled), samples(samples) {       }

void Base::Graphics::Graph2D::_drawAxes() {

    glLineWidth(1.0);

    __computeSpacings();

    __drawXAxis();
    __drawYAxis();

    if(!labels.empty()){
        const Real Sx = (xMax-xMin) / w;
        const Real Sy = (yMax-yMin) / h;
        const Real Tx = xMin;
        const Real Ty = yMin;

        for(size_t i=0; i<labels.size(); ++i){
            labels[i]->draw(Sx, Sy, Tx, Ty);
        }
    }
}

void Base::Graphics::Graph2D::__computeSpacings() {
    {
        const Real deltaY = yMax - yMin;

        const auto theLog = log10(deltaY);
        const auto spacing = pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        yspacing = multiplier * spacing;
    }

    {
        const auto deltaX = xMax - xMin;

        const auto theLog = log10(deltaX);
        const auto spacing = pow(10., floor(theLog) - 1.);
        const auto theRest = theLog - floor(theLog);
        const auto multiplier = floor(pow(10., theRest));

        xspacing = multiplier * spacing;
    }
}

void Base::Graphics::Graph2D::__drawXAxis() {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    const Real inPixelsTimes2 = 5;
    const Real vTick = inPixelsTimes2 * (yMax-yMin) / h;
    const Real hTick = inPixelsTimes2 * (xMax-xMin) / w;
    (void)hTick;

    auto &gtfColor = Styles::GetColorScheme()->graphTicksFont;

    {
        glColor4f(gtfColor.r, gtfColor.g, gtfColor.b, gtfColor.a);

        const Real yloc = -yspacing*0.356;
        for (Real mark = 0; mark <= xMax * 1.0001; mark += xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            GLUTUtils::writeOrtho(this, {xMin, xMax, yMin, yMax}, fontScale, mark - xspacing / 18.0, yloc, buffer, TICK_FONT);
        }
        for (Real mark = 0; mark >= xMin * 1.0001; mark -= xspacing) {
            char buffer[64];
            sprintf(buffer, "%.2f", mark);
            GLUTUtils::writeOrtho(this, {xMin, xMax, yMin, yMax}, fontScale, mark - xspacing / 18.0, yloc, buffer, TICK_FONT);
        }
    }

    if(1)
    {
        auto &ac = Styles::GetColorScheme()->axisColor;
        auto &tc = Styles::GetColorScheme()->majorTickColor;
        glBegin(GL_LINES);
        {
            glColor4f(ac.r, ac.g, ac.b, ac.a);

            glVertex3d(xMin, 0, 0);
            glVertex3d(xMax, 0, 0);

            glColor4f(tc.r, tc.g, tc.b, tc.a);

            for(Real mark = 0; mark<=xMax; mark+=xspacing){
                glVertex3d(mark, -vTick, 0);
                glVertex3d(mark, +vTick, 0);
            }
            for(Real mark = 0; mark>=xMin; mark-=xspacing){
                glVertex3d(mark, -vTick, 0);
                glVertex3d(mark, +vTick, 0);
            }
        }
        glEnd();
    }


}

void Base::Graphics::Graph2D::__drawYAxis() {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    const Real deltaY = yMax-yMin;
    const Real deltaX = xMax-xMin;
    const Real xMarkingsLabels = xMin - deltaX * 0.05;

    StringStream buffer;

    auto &gtf = Styles::GetColorScheme()->graphTicksFont;
    glColor4f(gtf.r, gtf.g, gtf.b, gtf.a);
    {
        if(yMin < 0 && yMax > 0) {
            for(Real mark = 0; mark>=yMin; mark-=yspacing)         MARK
            for(Real mark = yspacing; mark<=yMax; mark+=yspacing)  MARK
        } else {
            for (Real mark = yMin; mark <= yMax; mark += yspacing) MARK
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

        if(yMin < 0 && yMax > 0) {
            for(Real mark = 0; mark>=yMin; mark-=yspacing) {
                glVertex3d(xMin, mark, 0);
                glVertex3d(xMax, mark, 0);
            }
            for(Real mark = yspacing; mark<=yMax; mark+=yspacing) {
                glVertex3d(xMin, mark, 0);
                glVertex3d(xMax, mark, 0);
            }
        } else {
            for (Real mark = yMin; mark <= yMax; mark += yspacing) {
                glVertex3d(xMin, mark, 0);
                glVertex3d(xMax, mark, 0);
            }
        }

        glColor4f(ac.r, ac.g, ac.b, ac.a);

        glVertex3d(xMin, 0, 0);
        glVertex3d(xMax, 0, 0);
    }
    glEnd();
    glPopAttrib();

}

void Base::Graphics::Graph2D::setupOrtho() {
    const Real deltaX = xMax-xMin;
    const Real deltaY = yMax-yMin;
    const Real xTraLeft  = -deltaX*0.07;
    const Real xTraRight = +deltaX*0.02;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xMin+xTraLeft, xMax+xTraRight, (yMin-deltaY*0.025), (yMax+deltaY*0.025), -1, 1);
}

void Base::Graphics::Graph2D::_nameLabelDraw(int i, int j, const Styles::PlotStyle &style, Str label,
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
    auto colWidth = 0.5;
    auto xMin = .100+(colWidth+xGap+dx)*float(j),
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

        if(style.filled) {
            glBegin(GL_LINE_LOOP);
            glVertex2f(xMin, yMin);
            glVertex2f(xMax, yMin);
            glVertex2f(xMax, yMax);
            glVertex2f(xMin, yMax);
            glEnd();
        } else {
            glBegin(GL_LINES);
            glVertex2f(xMin, .5*(yMin+yMax));
            glVertex2f(xMax, .5*(yMin+yMax));
            glEnd();
        }

    }

    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1.5);

    auto c = Styles::GetColorScheme()->graphTitleFont;
    glColor4f(c.r,c.g,c.b,c.a);
    GLUTUtils::writeOrtho(window, Rect{0,1,0,1}, fontScale, xMax+xGap, .5*(yMax+yMin), label);

    glPopMatrix();
//
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void Base::Graphics::Graph2D::addLabel(Label *label) { labels.push_back(label); }

auto Base::Graphics::Graph2D::setResolution(Resolution samples_) -> void { this->samples = samples_; }

Rect Base::Graphics::Graph2D::getLimits() const {
    return Rect(xMin, xMax, yMin, yMax);
}

auto Base::Graphics::Graph2D::setLimits(Rect lims) -> void {
    xMin = lims.xMin;
    xMax = lims.xMax;
    yMin = lims.yMin;
    yMax = lims.yMax;
}

void Base::Graphics::Graph2D::set_xMin(Real val) { xMin = val; }
void Base::Graphics::Graph2D::set_xMax(Real val) { xMax = val; }
void Base::Graphics::Graph2D::set_yMin(Real val) { yMin = val; }
void Base::Graphics::Graph2D::set_yMax(Real val) { yMax = val; }
Real Base::Graphics::Graph2D::get_xMin() const { return xMin; }
Real Base::Graphics::Graph2D::get_xMax() const { return xMax; }
Real Base::Graphics::Graph2D::get_yMin() const { return yMin; }
Real Base::Graphics::Graph2D::get_yMax() const { return yMax; }

void
Base::Graphics::Graph2D::addCurve(RtoR2::ParametricCurve::Ptr curve, Styles::PlotStyle style, Str name) {
    CurveTriple triple = {curve, style, name};
    curves.emplace_back(triple);
}

void Base::Graphics::Graph2D::_drawCurves() {
    auto i=0;
    for(auto curveTriple : curves) {
        auto curve = GetCurve(curveTriple);
        auto pointSet = curve.get()->renderToPointSet();
        auto points = pointSet.get()->getPoints();

        if(points.size()<2) continue;

        auto style = GetStyle(curveTriple);
        auto name  = GetName(curveTriple);

        _nameLabelDraw(i, 1, style, name, this);

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

void Base::Graphics::Graph2D::draw() {
    Window::draw();

    setupOrtho();

    //glMatrixMode(GL_MODELVIEW);

    if(0)
    {
        auto &tf = Styles::GetColorScheme()->graphTitleFont;
        glColor4f(tf.r, tf.g, tf.b, tf.a);
        GLUTUtils::writeOrtho(this, {xMin, xMax, yMin, yMax}, fontScale*2., -0.95, 0.85, title, FONT_STROKE_ROMAN);
    }

    _drawAxes();

    _drawCurves();

    {
        auto popupName = Str("win_") + title + Str("_popup");

        if(savePopupOn) {
            ImGui::OpenPopup(popupName.c_str());
            savePopupOn = false;
        }

        if (ImGui::BeginPopup(popupName.c_str())){
            if(ImGui::MenuItem("Save graph")) {
                auto w = Printing::getTotalHorizontalDots(.5);
                auto h = w*.5;
                auto fileName = title + " " + InterfaceManager::getInstance().renderParametersToString({"N", "L"}) + ".png";
                OpenGLUtils::outputToPNG(this, fileName, w, h);
            }

            ImGui::EndPopup();
        }
    }
}

bool Base::Graphics::Graph2D::notifyMouseButton(int button, int dir, int x, int y) {
    if(0) Log::Debug() << "Window \"" << this->title << "\" mouse " << (dir==0 ? "clicked" : "released") << " button " << button << Log::Flush;

    static auto time = Timer();

    if(button == 2){
        if(dir == 0) time.reset();
        else if(dir == 1 && time.getElTime_msec() < 200) savePopupOn = true;

        return true;
    }

    return EventListener::notifyMouseButton(button, dir, x, y);
}

bool Base::Graphics::Graph2D::notifyMouseMotion(int x, int y) {
    auto elRet = EventListener::notifyMouseMotion(x, y);

    auto& mouseState = GUIBackend::GetInstance().getMouseState();

    if(mouseState.leftPressed)
    {
        const Real dxClampd = - mouseState.dx / (Real)w;
        const Real dyClampd = mouseState.dy / (Real)h;
        const Real wGraph = xMax-xMin;
        const Real hGraph = yMax-yMin;
        const Real dxGraph = wGraph * dxClampd;
        const Real dyGraph = hGraph * dyClampd;

        xMin += dxGraph;
        xMax += dxGraph;
        yMin += dyGraph;
        yMax += dyGraph;
    }
    if(mouseState.centerPressed)
    {
        constexpr const Real factor = 0.01;
        const Real dx = 1+factor*mouseState.dx;
        const Real dy = 1+factor*mouseState.dy;

        const Real x0 = .5*(xMax+xMin);
        const Real y0 = .5*(yMax+yMin);
        const Real hw = .5*(xMax-xMin)*dx;
        const Real hh = .5*(yMax-yMin)*dy;

        xMin = x0 - hw;
        xMax = x0 + hw;
        yMin = y0 - hh;
        yMax = y0 + hh;

    }

    return elRet;
}

bool Base::Graphics::Graph2D::notifyMouseWheel(int wheel, int direction, int x, int y) {
    EventListener::notifyMouseWheel(wheel, direction, x, y);

    constexpr const Real factor = 1.1;
    const Real d = pow(factor, -direction);

    if(GUIBackend::GetInstance().getMouseState().rightPressed) {
        const Real x0 = .5 * (xMax + xMin);
        const Real hw = .5 * (xMax - xMin) * d;
        xMin = x0 - hw;
        xMax = x0 + hw;
    } else {
        const Real y0 = .5 * (yMax + yMin);
        const Real hh = .5 * (yMax - yMin) * d;
        yMin = y0 - hh;
        yMax = y0 + hh;
    }

    return true;
}





