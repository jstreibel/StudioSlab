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

    auto domain = discreteFunc.getDomain();
    fix Δx = domain.xMax-domain.xMin;
    fix Δy = domain.yMax-domain.yMin;
    fix fraction = 0.2;
    set_xMin(domain.xMin - Δx*fraction);
    set_xMax(domain.xMax + Δx*fraction);
    set_yMin(domain.yMin - Δy*fraction);
    set_yMax(domain.yMax + Δy*fraction);

    auto xRes = discreteFunc.getN();
    auto yRes = discreteFunc.getM();

    texture = new OpenGL::Texture((int)xRes, (int)yRes);

    repopulateBuffer();
    validBuffer = true;
}

void R2toR::Graphics::FlatFieldDisplay::draw() {
    Base::Graphics::Graph2D::draw();

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

