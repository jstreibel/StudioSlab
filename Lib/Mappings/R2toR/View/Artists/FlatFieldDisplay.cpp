//
// Created by joao on 4/8/23.
//

#include "FlatFieldDisplay.h"

#include "Base/Graphics/Styles/ColorMap.h"
#include "Mappings/R2toR/Model/R2toRDiscreteFunction.h"
#include "imgui.h"

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

    computeGraphRanges();

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

    if(ImGui::Begin("Stats")){
        if(ImGui::CollapsingHeader("Full history output")) {

            {
                auto min = (float) cMap_min;
                auto max = (float) cMap_max;
                auto eps = (float) cMap_epsArg;

                if (ImGui::SliderFloat("min", &min, -2, -.005f)) {
                    cMap_min = min;
                    cMap_max = -min;
                    invalidateBuffer();
                }

                if (ImGui::SliderFloat("max", &max, .005f, 2)) {
                    cMap_max = max;
                    cMap_min = -max;
                    invalidateBuffer();
                }

                if (ImGui::DragFloat("log eps", &eps, eps * 1e-3, 1e-3, 1e3)) {
                    cMap_epsArg = eps;
                    if (logScale) invalidateBuffer();
                }

                if (ImGui::Checkbox("Log scale", &logScale)) {
                    invalidateBuffer();
                }
            }


            {
                StrVector items;
                for (const auto &cMapPair: Styles::ColorMaps)
                    items.emplace_back(cMapPair.first);
                static int item_current_idx = 0; // Here we store our selection data as an index.
                static int item_last_idx = 0;
                Str selectedItem;
                const char *combo_preview_value = items[item_current_idx].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
                if (ImGui::BeginCombo("Colormaps", combo_preview_value, 0)) {
                    for (int n = 0; n < items.size(); n++) {
                        const bool is_selected = (item_current_idx == n);
                        if (ImGui::Selectable(items[n].c_str(), is_selected)) {
                            selectedItem = items[n];
                            item_current_idx = n;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    if (item_last_idx != item_current_idx) {
                        cMap = Styles::ColorMaps[selectedItem];
                        invalidateBuffer();
                    }
                    item_last_idx = item_current_idx;
                    ImGui::EndCombo();
                }
            }
        }
    }
    ImGui::End();

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

    _drawAxes();
    _drawCurves();
}

void R2toR::Graphics::FlatFieldDisplay::invalidateBuffer() {
    validBuffer = false;
}

Styles::Color R2toR::Graphics::FlatFieldDisplay::computeColor(Real val) const {


    return cMap.mapValue(logScale ? logAbs(val, cMap_epsArg) : val, cMap_min, cMap_max);
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

void R2toR::Graphics::FlatFieldDisplay::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    computeGraphRanges();
}

void R2toR::Graphics::FlatFieldDisplay::computeGraphRanges() {
    if(func == nullptr) return;


    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);


    auto domain = discreteFunc.getDomain();

    fix dom_xMin = domain.xMin;
    fix dom_xMax = domain.xMax;
    fix dom_yMin = domain.yMin;
    fix dom_yMax = domain.yMax;

    fix fraction = 0.2;
    fix Δx = dom_xMax-dom_xMin;
    fix Δy = dom_yMax-dom_yMin;

    fix windowRatio = Window::geth()/(Real)Window::getw();
    fix fieldRatio = Δy/Δx;

    let _xMin = dom_xMin - Δx*fraction;
    let _xMax = dom_xMin + Δx*(1+fraction);
    let _yMin = dom_yMin - Δy*fraction;
    let _yMax = dom_yMin + Δy*(1+fraction);

    set_xMin(_xMin);
    set_xMax(_xMax);
    set_yMin(_yMin);
    set_yMax(_yMax/fieldRatio*windowRatio);
}

bool R2toR::Graphics::FlatFieldDisplay::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    auto retVal = Window::notifyScreenReshape(newScreenWidth, newScreenHeight);

    computeGraphRanges();

    return retVal;
}

