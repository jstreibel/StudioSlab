//
// Created by joao on 4/8/23.
//

#include "FlatFieldDisplay.h"

#include "Base/Graphics/Styles/ColorMap.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"

#include <utility>



R2toR::Graphics::FlatFieldDisplay::FlatFieldDisplay(R2toR::Function::ConstPtr function)
{
    setup(function);
}

void R2toR::Graphics::FlatFieldDisplay::setup(R2toR::Function::ConstPtr function) {
    func = std::move(function);
    if(!func->isDiscrete())
        throw "R2toR::Graphics::FlatFieldDisplay not-discrete function draw unimplemented";

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    fix ratio = Window::getw()/(Real)Window::geth();

    auto domain = discreteFunc.getDomain();

    fix dom_xMin = domain.xMin;
    fix dom_xMax = domain.xMax;
    fix dom_yMin = domain.yMin;
    fix dom_yMax = domain.yMax;

    fix fraction = 0.2;
    fix Δx = dom_xMax-dom_xMin;
    fix Δy = dom_yMax-dom_yMin;

    let _xMin = dom_xMin - Δx*fraction;
    let _xMax = dom_xMin + Δx*(1+fraction);
    let _yMin = dom_yMin - Δy*fraction;
    let _yMax = dom_yMin + Δy*(1+fraction);

    set_xMin(_xMin);
    set_xMax(_xMax);
    set_yMin(_yMin*ratio);
    set_yMax(_yMax*ratio);

    auto xRes = discreteFunc.getN();
    auto yRes = discreteFunc.getM();

    fontScale = .35;

    texture = new OpenGL::Texture((int)xRes, (int)yRes);

    repopulateBuffer();
    validBuffer = true;
}

void R2toR::Graphics::FlatFieldDisplay::draw() {
    Base::Graphics::Graph2D::draw();

    if(func == nullptr) return;

    if(!validBuffer) {
        repopulateBuffer();
        validBuffer = true;
    }

    glEnable(GL_TEXTURE_2D);
    glColor4d(1,1,1,1);
    glBegin(GL_QUADS);
    {
        auto p = .95f;

        auto ti = 0.f;
        auto tf = 1.f;

        auto domain = dynamic_cast<const R2toR::DiscreteFunction&>(*func).getDomain();

        glTexCoord2f(ti, ti); glVertex2f(domain.xMin, domain.yMin);
        glTexCoord2f(tf, ti); glVertex2f(domain.xMax, domain.yMin);
        glTexCoord2f(tf, tf); glVertex2f(domain.xMax, domain.yMax);
        glTexCoord2f(ti, tf); glVertex2f(domain.xMin, domain.yMax);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void R2toR::Graphics::FlatFieldDisplay::invalidateBuffer() {
    validBuffer = false;
}

void R2toR::Graphics::FlatFieldDisplay::repopulateBuffer() {
    if(func == nullptr) return;
    assert(func->isDiscrete());

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    auto xRes = discreteFunc.getN();
    auto yRes = discreteFunc.getM();

    for (int i = 0; i < xRes; ++i) {
        for (int j = 0; j < yRes; ++j) {
            auto val = discreteFunc.At(i, j);
            auto color = computeColor(val);

            texture->setColor(i, j, color);
        }
    }

    texture->upload();
}

Styles::Color R2toR::Graphics::FlatFieldDisplay::computeColor(Real val) const {
    static auto cMap = Styles::ColorMaps["BrBG"];

    return cMap.mapValue(logAbs(val, 0.1), -.2, .2);
}

bool R2toR::Graphics::FlatFieldDisplay::notifyMouseWheel(int wheel, int direction, int x, int y) {
    constexpr const Real factor = 1.1;
    const Real d = pow(factor, -direction);

    {
        const Real x0 = .5 * (xMax + xMin);
        const Real hw = .5 * (xMax - xMin) * d;
        xMin = x0 - hw;
        xMax = x0 + hw;

        const Real y0 = .5 * (yMax + yMin);
        const Real hh = .5 * (yMax - yMin) * d;
        yMin = y0 - hh;
        yMax = y0 + hh;
    }

    return true;
}

